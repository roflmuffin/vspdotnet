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

#pragma once

#include "core/global_listener.h"

class CCommand;

namespace vspdotnet {
class ChatCommands : public GlobalClass
{
public:
  ChatCommands() {}

  void OnAllInitialized() override;
  bool OnSayCommandPre(int client, CCommand* args);
  bool OnSayCommandPost(int client, CCommand* args);
 private:
  bool m_execute_in_post = false;
  const char* m_command_to_execute;
  char* m_args_backup;
};
}  // namespace vspdotnet