#include "core/autonative.h"
#include "core/callback_manager.h"
#include "core/script_engine.h"


namespace vspdotnet {

static bool AddListener(ScriptContext& script_context)
{
  auto name = script_context.GetArgument<const char*>(0);
  auto callback = script_context.GetArgument<CallbackT>(1);

  return globals::callback_manager.TryAddFunction(name, callback);
}

static bool RemoveListener(ScriptContext& script_context) {
  auto name = script_context.GetArgument<const char*>(0);
  auto callback = script_context.GetArgument<CallbackT>(1);

  return globals::callback_manager.TryRemoveFunction(name, callback);
}

REGISTER_NATIVES(callbacks, {
  ScriptEngine::RegisterNativeHandler("ADD_LISTENER", AddListener);
  ScriptEngine::RegisterNativeHandler("REMOVE_LISTENER", RemoveListener);
})
}
