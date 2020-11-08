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

#pragma once

#include <vector>

#include "core/convar_hack.h"
#include "core/globals.h"
#include "core/global_listener.h"
#include "core/script_engine.h"

namespace vspdotnet {

class CPluginConVarAccessor : public IConCommandBaseAccessor {
 public:
  virtual bool RegisterConCommandBase(ConCommandBase* pCommand) {
    globals::cvars->RegisterConCommand(pCommand);
    return true;
  }
};

extern CPluginConVarAccessor g_ConVarAccessor;


namespace convar
{
  static void InitServerCommands() { ConVar_Register(0, &g_ConVarAccessor); }
}

class ScriptCallback;

class ConVarInfo {
 private:
  ConVar* m_pConvar;
  bool m_is_created_by_sdn = false;
  ScriptCallback* m_change_callback;

 public:
  ConVarInfo(const char* name, const char* value, const char* description,
             int flags, bool hasMinValue, float min_value, bool hasMaxValue,
             float max_value)
      : m_change_callback(nullptr) {
    ConVar* pConVar = g_pCVar->FindVar(name);
    if (!pConVar) {
      ConVar* pConVar =
          new ConVar(strdup(name), strdup(value), flags, strdup(description),
                     hasMinValue, min_value, hasMaxValue, max_value);

      m_pConvar = pConVar;
      m_is_created_by_sdn = true;
      return;
    }

    m_pConvar = pConVar;
  }

  ConVarInfo(ConVar* pConVar) : m_change_callback(nullptr) {
    m_pConvar = pConVar;
  }

 protected:
  ConVarInfo() {}

 public:
  const char* GetStringValue() { return strdup(m_pConvar->GetString()); }

  float GetFloatValue() { return m_pConvar->GetFloat(); }

  int GetIntValue() { return m_pConvar->GetInt(); }

  bool GetBoolValue() { return m_pConvar->GetBool(); }

  void SetValue(const char* value) { m_pConvar->SetValue(value); }

  void SetValue(float flValue) { m_pConvar->SetValue(flValue); }
  void SetValue(int nValue) { m_pConvar->SetValue(nValue); }

  void SetValue(bool bValue) { m_pConvar->SetValue(bValue); }

  void SetFlags(int flags)
  {
    m_pConvar->m_nFlags = flags;
  }

  int GetFlags() { return m_pConvar->GetFlags(); }

  void MakePublic() {
    m_pConvar->m_nFlags |= FCVAR_NOTIFY;
    g_pCVar->CallGlobalChangeCallbacks(m_pConvar, m_pConvar->GetString(),
                                       m_pConvar->GetFloat());
  }

  void RemovePublic() {
    m_pConvar->m_nFlags &= ~FCVAR_NOTIFY;
    g_pCVar->CallGlobalChangeCallbacks(m_pConvar, m_pConvar->GetString(),
                                       m_pConvar->GetFloat());
  }

  bool IsCustom() { return m_is_created_by_sdn; }

  const char* GetName() { return m_pConvar->GetName(); }

  ConVar* GetConVar() { return m_pConvar; }

  ScriptCallback* GetCallback() { return m_change_callback; }

  void SetCallback(ScriptCallback* callback) {
    m_change_callback = callback;
  }
};

class ConVarManager : public GlobalClass {
 public:
  ConVarManager();
  ~ConVarManager();

 public:
  void OnAllInitialized() override;
  void OnShutdown();
  void OnConVarChanged(ConVar* pConvar, const char* oldValue, float flOldValue);

 public:
  ConVarInfo* CreateConVar(const char* name, const char* value,
                           const char* description, int flags, bool hasMinValue,
                           float min_value, bool hasMaxValue, float max_value);
  ConVarInfo* FindConVar(const char* name);
  bool Unregister(ConVarInfo* name);
  void HookConVarChange(ConVarInfo* pConvar, CallbackT callback);
  void UnhookConVarChange(ConVarInfo* pConvar, CallbackT callback);

 private:
  std::vector<ConVarInfo*> m_ConVars;
  void AddToCache(ConVarInfo* convar);
};

}  // namespace vspdotnet
