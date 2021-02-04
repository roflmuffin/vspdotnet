#include "core/timer_system.h"

#include <public/eiface.h>

#include "core/callback_manager.h"
#include "core/globals.h"
#include "core/log.h"


namespace vspdotnet
{
namespace timers
{
double universal_time = 0.0f;
double timer_next_think = 0.0f;
}

timers::Timer::Timer(float interval, float exec_time, CallbackT callback, int flags)
  : m_interval(interval),
    m_exec_time(exec_time),
    m_flags(flags),
    m_kill_me(false),
    m_in_exec(false)
{
  m_callback = globals::callback_manager.CreateCallback("Timer");
  m_callback->AddListener(callback);
}

timers::Timer::~Timer()
{
  globals::callback_manager.ReleaseCallback(m_callback);
}

TimerSystem::TimerSystem()
{
  m_has_map_ticked = false;
  m_has_map_simulated = false;
  m_last_ticked_time = 0.0f;
}

void TimerSystem::OnAllInitialized()
{
  m_on_tick_callback_ = globals::callback_manager.CreateCallback("OnTick");
}

void TimerSystem::OnLevelEnd()
{
  m_has_map_simulated = false;
  m_has_map_ticked = false;
}

void TimerSystem::OnGameFrame(bool simulating)
{
  if (simulating && m_has_map_ticked)
  {
    timers::universal_time += globals::gpGlobals->curtime - m_last_ticked_time;
    if (!m_has_map_simulated)
    {
      m_has_map_simulated = true;
    }
  }
  else
  {
    timers::universal_time += globals::gpGlobals->interval_per_tick;
  }

  m_last_ticked_time = globals::gpGlobals->curtime;
  m_has_map_ticked = true;

  // Handle timer tick
  if (timers::universal_time >= timers::timer_next_think)
  {
    RunFrame();

    timers::timer_next_think = CalculateNextThink(timers::timer_next_think, 0.1f);
  }

  if (m_on_tick_callback_->GetFunctionCount())
  {
    m_on_tick_callback_->ScriptContext().Reset();
    m_on_tick_callback_->Execute();
  }
}

double TimerSystem::CalculateNextThink(double last_think_time, float interval)
{
  if (timers::universal_time - last_think_time - interval <= 0.1)
  {
    return last_think_time + interval;
  }
  else
  {
    return timers::universal_time + interval;
  }
}

void TimerSystem::RunFrame()
{
  VSPDN_CORE_TRACE("Running timer ticks at time: {0}, has_map_simulated: {1}, has_map_ticked: {2}",
                  timers::universal_time, m_has_map_simulated, m_has_map_ticked);

  for (int i = m_once_off_timers.size() - 1; i >= 0; i--)
  {
    auto timer = m_once_off_timers[i];
    if (timers::universal_time >= timer->m_exec_time)
    {
      timer->m_in_exec = true;
      timer->m_callback->ScriptContext().Reset();
      timer->m_callback->Execute();

      m_once_off_timers.erase(m_once_off_timers.begin() + i);
      delete timer;
    }
  }

  for (int i = m_repeat_timers.size() - 1; i >= 0; i--) 
  {
    auto timer = m_repeat_timers[i];
    if (timers::universal_time >= timer->m_exec_time)
    {
      timer->m_in_exec = true;
      timer->m_callback->ScriptContext().Reset();
      timer->m_callback->Execute();

      if (timer->m_kill_me)
      {
        m_repeat_timers.erase(m_repeat_timers.begin() + i);
        delete timer;
        continue;
      }

      timer->m_in_exec = false;
      timer->m_exec_time = CalculateNextThink(timer->m_exec_time, timer->m_interval);
    }
  }
}

void TimerSystem::RemoveMapChangeTimers()
{
  for (auto timer : m_once_off_timers)
  {
    if (timer->m_flags & TIMER_FLAG_NO_MAPCHANGE)
    {
      KillTimer(timer);
    }
  }

  for (auto timer : m_repeat_timers)
  {
    if (timer->m_flags & TIMER_FLAG_NO_MAPCHANGE)
    {
      KillTimer(timer);
    }
  }
}

timers::Timer* TimerSystem::CreateTimer(float interval, CallbackT callback, int flags)
{
  float exec_time = timers::universal_time + interval;

  auto timer = new timers::Timer(interval, exec_time, callback, flags);

  if (flags & TIMER_FLAG_REPEAT)
  {
    m_repeat_timers.push_back(timer);
    return timer;
  }

  m_once_off_timers.push_back(timer);
  return timer;
}


void TimerSystem::KillTimer(timers::Timer* timer)
{
  if (!timer) return;

  if (std::find(m_repeat_timers.begin(), m_repeat_timers.end(), timer) ==
          m_repeat_timers.end() &&
      std::find(m_once_off_timers.begin(), m_once_off_timers.end(), timer) ==
          m_once_off_timers.end()) {
    return;
  } 

  if (timer->m_kill_me) return;

  // If were executing, make sure it doesn't run again next time.
  if (timer->m_in_exec)
  {
    timer->m_kill_me = true;
    return;
  }

  if (timer->m_flags & TIMER_FLAG_REPEAT)
  {
    auto it = std::remove_if(m_repeat_timers.begin(), m_repeat_timers.end(),
                             [timer](timers::Timer* i) { return timer == i; });

    bool success;
    if ((success = it != m_repeat_timers.end()))
      m_repeat_timers.erase(it, m_repeat_timers.end());
    delete timer;
  }
  else
  {
    auto it = std::remove_if(m_once_off_timers.begin(), m_once_off_timers.end(),
                             [timer](timers::Timer* i) { return timer == i; });

    bool success;
    if ((success = it != m_once_off_timers.end()))
      m_once_off_timers.erase(it, m_once_off_timers.end());
    delete timer;
  }
}

double TimerSystem::GetTickedTime()
{ return timers::universal_time; }
} // namespace vspdotnet
