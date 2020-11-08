/*
 *  This file is part of VSP.NET.
 *  VSP.NET is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  VSP.NET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with VSP.NET.  If not, see <https://www.gnu.org/licenses/>. *
 */

#include <game/shared/csgo/protobuf/cstrike15_usermessage_helpers.h>

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

int GetUserMessageId(ScriptContext& script_context)
{
  auto name = script_context.GetArgument<const char*>(0);
  return g_Cstrike15UsermessageHelpers.GetIndex(name);
}

REGISTER_NATIVES(usermessages, {
  ScriptEngine::RegisterNativeHandler("PRINT_TO_CHAT", PrintToChat);
  ScriptEngine::RegisterNativeHandler("PRINT_TO_CONSOLE", PrintToConsole);
  ScriptEngine::RegisterNativeHandler("PRINT_TO_HINT", PrintToHint);
  ScriptEngine::RegisterNativeHandler("PRINT_TO_CENTER", PrintToCenter);
  ScriptEngine::RegisterNativeHandler("GET_USER_MESSAGE_ID", GetUserMessageId);
})
}