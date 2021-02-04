#include <core/autonative.h>
#include <core/script_engine.h>

#include "core/timer_system.h"

namespace vspdotnet
{

timers::Timer* CreateTimer(ScriptContext& script_context)
{
  auto interval = script_context.GetArgument<float>(0);
  auto callback = script_context.GetArgument<CallbackT>(1);
  auto flags = script_context.GetArgument<int>(2);

  return globals::timer_system.CreateTimer(interval, callback, flags);
}

void KillTimer(ScriptContext& script_context)
{
  auto timer = script_context.GetArgument<timers::Timer*>(0);
  globals::timer_system.KillTimer(timer);
}

double GetTickedTime(ScriptContext& script_context)
{ return globals::timer_system.GetTickedTime(); }

REGISTER_NATIVES(timers, {
  ScriptEngine::RegisterNativeHandler("GET_TICKED_TIME", GetTickedTime);
  ScriptEngine::RegisterNativeHandler("CREATE_TIMER", CreateTimer);
  ScriptEngine::RegisterNativeHandler("KILL_TIMER", KillTimer);
})
}
