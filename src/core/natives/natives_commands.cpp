#include "core/autonative.h"
#include "core/callback_manager.h"
#include "core/script_engine.h"
#include "core/con_command_manager.h"

namespace vspdotnet {

static ConCommandInfo* AddCommand(ScriptContext& script_context) {
  auto name = script_context.GetArgument<const char*>(0);
  auto description = script_context.GetArgument<const char*>(1);
  auto server_only = script_context.GetArgument<bool>(2);
  auto flags = script_context.GetArgument<int>(3);
  auto callback = script_context.GetArgument<CallbackT>(4);
  
  return globals::con_command_manager.AddCommand(name, description, server_only,
                                                 flags, callback);
}

static void RemoveCommand(ScriptContext& script_context)
{
  auto name = script_context.GetArgument<const char*>(0);
  auto callback = script_context.GetArgument<CallbackT>(1);

  globals::con_command_manager.RemoveCommand(name, callback);
}

static int CommandGetArgCount(ScriptContext& script_context) {
  auto command = script_context.GetArgument<CCommand*>(0);

  if (!command)
  {
    script_context.ThrowNativeError("Invalid command.");
    return -1;
  }

  return command->ArgC();
}

static const char* CommandGetArgString(ScriptContext& script_context) {
  auto command = script_context.GetArgument<CCommand*>(0);

  if (!command) {
    script_context.ThrowNativeError("Invalid command.");
    return nullptr;
  }

  return command->ArgS();
}

static const char* CommandGetCommandString(ScriptContext& script_context) {
  auto* command = script_context.GetArgument<CCommand*>(0);

  if (!command) {
    script_context.ThrowNativeError("Invalid command.");
    return nullptr;
  }

  return command->GetCommandString();
}

static const char* CommandGetArgByIndex(ScriptContext& script_context) {
  auto* command = script_context.GetArgument<CCommand*>(0);
  auto index = script_context.GetArgument<int>(1);

  if (!command) {
    script_context.ThrowNativeError("Invalid command.");
    return nullptr;
  }

  return command->Arg(index);
}

REGISTER_NATIVES(commands, {
  ScriptEngine::RegisterNativeHandler("ADD_COMMAND", AddCommand);
  ScriptEngine::RegisterNativeHandler("REMOVE_COMMAND", RemoveCommand);

  ScriptEngine::RegisterNativeHandler("COMMAND_GET_ARG_COUNT", CommandGetArgCount);
  ScriptEngine::RegisterNativeHandler("COMMAND_GET_ARG_STRING", CommandGetArgString);
  ScriptEngine::RegisterNativeHandler("COMMAND_GET_COMMAND_STRING", CommandGetCommandString);
  ScriptEngine::RegisterNativeHandler("COMMAND_GET_ARG_BY_INDEX", CommandGetArgByIndex);
})
}