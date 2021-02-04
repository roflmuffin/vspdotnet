#include <memory>
#include <vector>

#include "script_engine.h"
#include "core/global_listener.h"

namespace vspdotnet
{
class TimerSystem;
class ScriptCallback;

namespace timers
{
  #define TIMER_FLAG_REPEAT (1 << 0)       /**< Timer will repeat until stopped */
  #define TIMER_FLAG_NO_MAPCHANGE (1 << 1) /**< Timer will not carry over mapchanges */

  class Timer
  {
  friend class TimerSystem;
  public:
  Timer(float interval, float exec_time, CallbackT callback, int flags);
   ~Timer();
  private:
    float m_interval;
    ScriptCallback* m_callback;
    int m_flags;
    float m_exec_time;
    bool m_in_exec;
    bool m_kill_me;
  };

}

class ScriptCallback;

class TimerSystem : public GlobalClass
  {
  public:
    TimerSystem();
    void OnAllInitialized() override;
    void OnLevelEnd() override;
    void OnGameFrame(bool simulating);
    double CalculateNextThink(double last_think_time, float interval);
    void RunFrame();
    void RemoveMapChangeTimers();
    timers::Timer* CreateTimer(float interval, CallbackT callback, int flags);
    void KillTimer(timers::Timer* timer);
    double GetTickedTime();
  private:
    bool m_has_map_ticked = false;
    bool m_has_map_simulated = false;
    float m_last_ticked_time = 0.0f;
    ScriptCallback* m_on_tick_callback_ = nullptr;

    std::vector<timers::Timer*> m_once_off_timers;
    std::vector<timers::Timer*> m_repeat_timers;
  };
}
