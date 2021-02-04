#include "core/autonative.h"
#include "core/script_engine.h"
#include "core/user_message_manager.h"

namespace vspdotnet
{

void PrintToChat(ScriptContext &script_context) {
  int index = script_context.GetArgument<int>(0);
  const char *message = script_context.GetArgument<const char *>(1);

  globals::user_message_manager.SendMessageToChat(index, message);
}

void PrintToConsole(ScriptContext &script_context) {
  const char *message = script_context.GetArgument<const char *>(0);

  Msg("%s", message);
}

void PrintToHint(ScriptContext &script_context) {
  int index = script_context.GetArgument<int>(0);
  const char *message = script_context.GetArgument<const char *>(1);

  globals::user_message_manager.SendHintMessage(index, message);
}

void PrintToCenter(ScriptContext &script_context) {
  int index = script_context.GetArgument<int>(0);
  const char *message = script_context.GetArgument<const char *>(1);

  globals::user_message_manager.SendCenterMessage(index, message);
}

REGISTER_NATIVES(usermessages, {
  ScriptEngine::RegisterNativeHandler("PRINT_TO_CHAT", PrintToChat);
  ScriptEngine::RegisterNativeHandler("PRINT_TO_CONSOLE", PrintToConsole);
  ScriptEngine::RegisterNativeHandler("PRINT_TO_HINT", PrintToHint);
  ScriptEngine::RegisterNativeHandler("PRINT_TO_CENTER", PrintToCenter);
})
}