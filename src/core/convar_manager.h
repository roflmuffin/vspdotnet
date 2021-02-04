#ifndef _CONVAR_MANAGER
#define _CONVAR_MANAGER

#include <vector>

#include "core/globals.h"
#include "core/convar_hack.h"

#include "script_engine.h"

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
  ScriptCallback* m_change_callback_;

 public:
  ConVarInfo(const char* name, const char* value, const char* description,
             int flags, bool hasMinValue, float min_value, bool hasMaxValue,
             float max_value)
      : m_change_callback_(nullptr) {
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

  ConVarInfo(ConVar* pConVar) : m_change_callback_(nullptr) {
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

  ScriptCallback* GetCallback() { return m_change_callback_; }

  void SetCallback(ScriptCallback* callback) {
    m_change_callback_ = callback;
  }
};

class ConVarManager : public GlobalClass {
#ifndef SWIG
 public:
  ConVarManager();
  ~ConVarManager();

 public:
  void OnAllInitialized() override;
  void OnShutdown();
  void OnConVarChanged(ConVar* pConvar, const char* oldValue, float flOldValue);
#endif

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
#endif  // !_CONVAR_MANAGER