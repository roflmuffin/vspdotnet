#include "core/autonative.h"
#include "core/convar_manager.h"

namespace vspdotnet {

ConVarInfo* FindConVar(ScriptContext& script_context)
{
  const auto name = script_context.GetArgument<const char*>(0);

  return globals::convar_manager.FindConVar(name);
}

ConVarInfo* CreateConVar(ScriptContext& script_context)
{
  const auto name = script_context.GetArgument<const char*>(0);
  const auto value = script_context.GetArgument<const char*>(1);
  const auto description = script_context.GetArgument<const char*>(2);
  const auto flags = script_context.GetArgument<int>(3);
  const auto hasMinValue = script_context.GetArgument<bool>(4);
  const auto minValue = script_context.GetArgument<float>(5);
  const auto hasMaxValue = script_context.GetArgument<bool>(6);
  const auto maxValue = script_context.GetArgument<float>(7);

  return globals::convar_manager.CreateConVar(name, value, description, flags,
                                       hasMinValue, minValue, hasMaxValue,
                                     maxValue);
}

void HookConVarChange(ScriptContext& script_context)
{
  auto convar = script_context.GetArgument<ConVarInfo*>(0);
  auto callback = script_context.GetArgument<CallbackT>(1);
  globals::convar_manager.HookConVarChange(convar, callback);
}

void UnhookConVarChange(ScriptContext& script_context) {
  auto convar = script_context.GetArgument<ConVarInfo*>(0);
  auto callback = script_context.GetArgument<CallbackT>(1);
  globals::convar_manager.UnhookConVarChange(convar, callback);
}

int ConVarGetFlags(ScriptContext& script_context)
{
  auto convar = script_context.GetArgument<ConVarInfo*>(0);

  if (!convar)
  {
    script_context.ThrowNativeError("Invalid ConVar");
  }

  return convar->GetFlags();
}

void ConVarSetFlags(ScriptContext& script_context) {
  auto convar = script_context.GetArgument<ConVarInfo*>(0);
  auto flags = script_context.GetArgument<int>(1);

  if (!convar) {
    script_context.ThrowNativeError("Invalid ConVar");
  }

  convar->SetFlags(flags);
}

const char* ConVarGetStringValue(ScriptContext& script_context)
{
  auto convar = script_context.GetArgument<ConVarInfo*>(0);

  if (!convar) {
    script_context.ThrowNativeError("Invalid ConVar");
  }

  return convar->GetStringValue();
}

void ConVarSetStringValue(ScriptContext& script_context) {
  auto convar = script_context.GetArgument<ConVarInfo*>(0);
  auto value = script_context.GetArgument<const char*>(1);

  if (!convar) {
    script_context.ThrowNativeError("Invalid ConVar");
  }

  convar->SetValue(value);
}

const char* ConVarGetName(ScriptContext& script_context)
{
  auto convar = script_context.GetArgument<ConVarInfo*>(0);

  if (!convar) {
    script_context.ThrowNativeError("Invalid ConVar");
    return nullptr;
  }

  return convar->GetName();
}

void ConVarUnregister(ScriptContext& script_context)
{
  auto convar = script_context.GetArgument<ConVarInfo*>(0);

  if (!convar) {
    script_context.ThrowNativeError("Invalid ConVar");
  }

  globals::convar_manager.Unregister(convar);
}

REGISTER_NATIVES(convars, {
  ScriptEngine::RegisterNativeHandler("FIND_CONVAR", FindConVar);
  ScriptEngine::RegisterNativeHandler("CREATE_CONVAR", CreateConVar);
  ScriptEngine::RegisterNativeHandler("HOOK_CONVAR_CHANGE", HookConVarChange);
  ScriptEngine::RegisterNativeHandler("UNHOOK_CONVAR_CHANGE", UnhookConVarChange);
  ScriptEngine::RegisterNativeHandler("CONVAR_GET_FLAGS", ConVarGetFlags);
  ScriptEngine::RegisterNativeHandler("CONVAR_SET_FLAGS", ConVarSetFlags);
  ScriptEngine::RegisterNativeHandler("CONVAR_GET_STRING_VALUE", ConVarGetStringValue);
  ScriptEngine::RegisterNativeHandler("CONVAR_SET_STRING_VALUE", ConVarSetStringValue);
  ScriptEngine::RegisterNativeHandler("CONVAR_GET_NAME", ConVarGetName);
  ScriptEngine::RegisterNativeHandler("CONVAR_UNREGISTER", ConVarUnregister);
})
}
