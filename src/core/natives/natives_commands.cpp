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

#include <eiface.h>

#include "core/autonative.h"
#include "core/callback_manager.h"
#include "core/con_command_manager.h"
#include "core/entity.h"
#include "core/player_manager.h"
#include "core/script_engine.h"
#include "core/utilities/conversions.h"

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

static void IssueClientCommand(ScriptContext& script_context)
{
  auto entity_index = script_context.GetArgument<int>(0);
  auto command = script_context.GetArgument<const char*>(1);

  CBaseEntity* base_entity;
  if (!BaseEntityFromIndex(entity_index, base_entity)) {
    script_context.ThrowNativeError("Entity %d is invalid", entity_index);
    return;
  }

  if (entity_index > 0 && entity_index <= globals::gpGlobals->maxClients) {
    auto player = globals::player_manager.GetPlayerByIndex(entity_index);
    if (!player || !player->IsConnected()) return;
  }

  auto wrapper_entity = reinterpret_cast<CBaseEntityWrapper*>(base_entity);

  globals::engine->ClientCommand(wrapper_entity->GetEdict(), command);
}


REGISTER_NATIVES(commands, {
  ScriptEngine::RegisterNativeHandler("ADD_COMMAND", AddCommand);
  ScriptEngine::RegisterNativeHandler("REMOVE_COMMAND", RemoveCommand);

  ScriptEngine::RegisterNativeHandler("COMMAND_GET_ARG_COUNT", CommandGetArgCount);
  ScriptEngine::RegisterNativeHandler("COMMAND_GET_ARG_STRING", CommandGetArgString);
  ScriptEngine::RegisterNativeHandler("COMMAND_GET_COMMAND_STRING", CommandGetCommandString);
  ScriptEngine::RegisterNativeHandler("COMMAND_GET_ARG_BY_INDEX", CommandGetArgByIndex);

  ScriptEngine::RegisterNativeHandler("ISSUE_CLIENT_COMMAND", IssueClientCommand);
})
}
