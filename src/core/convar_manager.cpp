/**
 * =============================================================================
 * SourceMod
 * Copyright (C) 2004-2016 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * This file has been modified from its original form, under the GNU General
 * Public License, version 3.0.
 */


#include "core/convar_manager.h"

#include <sourcehook/sourcehook.h>

#include <algorithm>
#include <map>
#include <string>

#include "core/callback_manager.h"


SH_DECL_HOOK3_void(ICvar, CallGlobalChangeCallbacks, SH_NOATTRIB, false,
                   ConVar*, const char*, float);

namespace vspdotnet {

CPluginConVarAccessor g_ConVarAccessor;

std::map<std::string, ConVarInfo*> convar_cache;

ConVarManager::ConVarManager() {}

ConVarManager::~ConVarManager() {}

void ConVarManager::OnAllInitialized() {
  SH_ADD_HOOK(ICvar, CallGlobalChangeCallbacks, g_pCVar,
              SH_MEMBER(this, &ConVarManager::OnConVarChanged), false);
}

void ConVarManager::OnShutdown() {
  SH_REMOVE_HOOK(ICvar, CallGlobalChangeCallbacks, g_pCVar,
                 SH_MEMBER(this, &ConVarManager::OnConVarChanged), false);
}

bool convar_cache_lookup(const char* name, ConVarInfo** pVar) {
  auto search = convar_cache.find(name);
  if (search != convar_cache.end()) {
    *pVar = search->second;
    return true;
  }

  return false;
}

void ConVarManager::OnConVarChanged(ConVar* pConVar, const char* oldValue,
                                    float flOldValue) {
  if (strcmp(pConVar->GetString(), oldValue) == 0) {
    return;
  }

  ConVarInfo* pInfo;

  if (!convar_cache_lookup(pConVar->GetName(), &pInfo)) {
    return;
  }

  ScriptCallback* callback = pInfo->GetCallback();
  if (callback != nullptr) {
    callback->ScriptContext().Reset();
    callback->ScriptContext().SetArgument(0, pInfo);
    callback->ScriptContext().SetArgument(1, oldValue);
    callback->ScriptContext().SetArgument(2, pInfo->GetStringValue());
    callback->Execute();
  }
}

void ConVarManager::HookConVarChange(ConVarInfo* pInfo, CallbackT callback) {
  ScriptCallback* p_callback;

  /* Find the convar in the lookup trie */
  if (convar_cache_lookup(pInfo->GetName(), &pInfo)) {
    /* Get the forward */
    p_callback = pInfo->GetCallback();

    /* If forward does not exist, create it */
    if (!p_callback) {
      p_callback = globals::callback_manager.CreateCallback(pInfo->GetName());

      pInfo->SetCallback(p_callback);
    }

    p_callback->AddListener(callback);
  }
}

void ConVarManager::UnhookConVarChange(ConVarInfo* pInfo, CallbackT callback) {
  ScriptCallback* p_callback;

  /* Find the convar in the lookup trie */
  if (convar_cache_lookup(pInfo->GetName(), &pInfo)) {
    /* Get the forward */
    p_callback = pInfo->GetCallback();

    if (!p_callback) {
      return;
    }

    p_callback->RemoveListener(callback);
  }
}

bool ConVarManager::Unregister(ConVarInfo* pInfo) {
  ScriptCallback* p_callback;

  if (convar_cache_lookup(pInfo->GetName(), &pInfo)) {
    /* Get the forward */
    p_callback = pInfo->GetCallback();

    if (pInfo->GetCallback() && pInfo->GetCallback()->GetFunctionCount())
      return false;

    convar_cache[pInfo->GetName()] = nullptr;

    bool success;
    auto it = std::remove_if(m_ConVars.begin(), m_ConVars.end(),
                             [pInfo](ConVarInfo* i) { return pInfo == i; });

    if ((success = it != m_ConVars.end())) m_ConVars.erase(it, m_ConVars.end());

    if (success) {
      convar_cache[pInfo->GetName()] = nullptr;

      if (pInfo->IsCustom()) {
        g_pCVar->UnregisterConCommand(pInfo->GetConVar());
      }

      pInfo = nullptr;

      return true;
    }
  }

  return false;
}

ConVarInfo* ConVarManager::CreateConVar(const char* name, const char* value,
                                        const char* description, int flags,
                                        bool hasMinValue, float min_value,
                                        bool hasMaxValue, float max_value) {
  ConVarInfo* pInfo =
      new ConVarInfo(name, value, description, flags, hasMinValue, min_value,
                     hasMaxValue, max_value);
  m_ConVars.push_back(pInfo);
  convar_cache.insert_or_assign(name, pInfo);

  return pInfo;
}

ConVarInfo* ConVarManager::FindConVar(const char* name) {
  ConVarInfo* pInfo;
  if (convar_cache_lookup(name, &pInfo)) {
    return pInfo;
  }

  ConVar* pConVar = g_pCVar->FindVar(name);
  if (!pConVar) return nullptr;

  pInfo = new ConVarInfo(pConVar);

  m_ConVars.push_back(pInfo);
  convar_cache.insert_or_assign(name, pInfo);

  return pInfo;
}

void ConVarManager::AddToCache(ConVarInfo* convar) {
  m_ConVars.push_back(convar);
  convar_cache.insert_or_assign(convar->GetName(), convar);
}

}  // namespace vspdotnet