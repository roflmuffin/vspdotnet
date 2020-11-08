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

#include <hook.h>

#include <map>
#include <vector>

#include "core/autonative.h"
#include "core/global_listener.h"

class CHook;

namespace vspdotnet {
  
class ValveFunction;

struct EntityHook
{
  int index;
  CallbackT callback;
};

class HookManager : public GlobalClass
{
public:
  void OnAllInitialized() override;
  void AddHook(ValveFunction* function, int index, CallbackT callback, bool is_post);
  void Unhook(ValveFunction* function, int index, CallbackT callback, bool is_post);
  bool HookHandler(HookType_t, CHook*);
 private:
  std::map<CHook*, std::map<HookType_t, std::vector<EntityHook>>> m_callback_map;

  // Hack to copy the this pointer on windows.
  std::unordered_map<void*, void*> m_ecx_addresses;
};}
