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
