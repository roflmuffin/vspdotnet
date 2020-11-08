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

#include "core/chat_commands.h"

#include <public/engine/iserverplugin.h>

#include "core/con_command_manager.h"
#include "core/globals.h"
#include "core/log.h"
#include "core/player_manager.h"
#include "core/utilities/conversions.h"

namespace vspdotnet
{
void ChatCommands::OnAllInitialized()
{
  auto say = globals::con_command_manager.FindCommand("say");
  auto say_team = globals::con_command_manager.FindCommand("say_team");

  auto pre_hook = [](fxNativeContext* cxt)
  {
    auto context = ScriptContextRaw(*cxt);
    auto client = context.GetArgument<int>(0);
    auto args = context.GetArgument<CCommand*>(1);
    context.SetResult(globals::chat_commands.OnSayCommandPre(client, args));
  };

  auto post_hook = [](fxNativeContext* cxt)
  {
    auto context = ScriptContextRaw(*cxt);
    auto client = context.GetArgument<int>(0);
    auto args = context.GetArgument<CCommand*>(1);
    context.SetResult(globals::chat_commands.OnSayCommandPost(client, args));
  };

  say->HookChange(pre_hook, false);
  say->HookChange(post_hook, true);

  say_team->HookChange(pre_hook, false);
  say_team->HookChange(post_hook, true);
}

bool ChatCommands::OnSayCommandPre(int client, CCommand* command)
{
  const char* args = command->ArgS();

  if (!args) return false;

  if (client == 0) return false;

  auto player = globals::player_manager.GetPlayerByIndex(client);

  if (!player || !player->IsConnected()) return false;

  bool is_trigger = false;
  bool is_silent = false;

  bool is_quoted = false;

  auto len = strlen(args);
  if (client != 0 && args[0] == '"' && args[len - 1] == '"')
  {
    if (len <= 2) return true;

    args++;
    len--;
    is_quoted = true;
  }

  delete[] m_args_backup;
  m_args_backup = new char[CCommand::MaxCommandLength() + 1];
  memcpy(m_args_backup, args, len + 1);

  if (is_quoted)
  {
    if (m_args_backup[len - 1] == '"')
    {
      m_args_backup[--len] = '\0';
    }
  }

  if (strchr("/", m_args_backup[0]))
  {
    is_trigger = true;
    is_silent = true;
  } else if (strchr("!", m_args_backup[0]))
  {
    is_trigger = true;
  }

  if (is_trigger)
  {
    args = &m_args_backup[1];
  }

  if (is_trigger)
  {
    m_execute_in_post = true;
    m_command_to_execute = args;
  }

  if (is_silent)
  {
    return true;
  }

  return false;
}

bool ChatCommands::OnSayCommandPost(int client, CCommand* args) {
  if (m_execute_in_post) {
    m_execute_in_post = false;

    globals::helpers->ClientCommand(ExcEdictFromIndex(client), m_command_to_execute);
  }

  return false;
}

} // namespace vspdotnet
