#include "callback_manager.h"
#include "core/log.h"

namespace vspdotnet {

ScriptCallback::ScriptCallback(const char* name) : m_root_context(fxNativeContext{}) {
  m_script_context_raw = ScriptContextRaw(m_root_context);
  m_name = std::string(name);
}

ScriptCallback::~ScriptCallback()
{ m_functions.clear(); }


void ScriptCallback::AddListener(CallbackT plugin_function) {
  m_functions.push_back(plugin_function);
}


bool ScriptCallback::RemoveListener(CallbackT plugin_function) {
  bool success;
  auto it = std::remove_if(m_functions.begin(), m_functions.end(),
      [plugin_function](void* i)
      {
        return plugin_function == i;
      });

  if ((success = it != m_functions.end()))
    m_functions.erase(it, m_functions.end());

  return success;
}

void ScriptCallback::Execute(bool resetContext)
{
  for (auto method_to_call : m_functions) {
    if (method_to_call) {
      method_to_call(&ScriptContextStruct());
    }
  }

  if (resetContext) {
    ResetContext();
  }
}

void ScriptCallback::ResetContext()
{
  ScriptContext().Reset();
}

CallbackManager::CallbackManager() {}


ScriptCallback* CallbackManager::CreateCallback(const char* name)
{
  auto* callback = new ScriptCallback(name);
  m_managed.push_back(callback);

  return callback;
}

ScriptCallback* CallbackManager::FindCallback(const char* name) {
  for (auto it = m_managed.begin(); it != m_managed.end(); ++it) {
    ScriptCallback* marshal = *it;
    if (strcmp(marshal->GetName().c_str(), name) == 0) {
      return marshal;
    }
  }

  return nullptr;
}

void CallbackManager::ReleaseCallback(ScriptCallback* callback) {
  bool success;
  auto it = std::remove_if(
      m_managed.begin(), m_managed.end(),
                           [callback](ScriptCallback* i) { return callback == i; });

  if ((success = it != m_managed.end()))
    m_managed.erase(it, m_managed.end());
  delete callback;
}

bool CallbackManager::TryAddFunction(const char* name, CallbackT pCallable) {
  auto* fwd = FindCallback(name);
  if (fwd) {
    fwd->AddListener(pCallable);
    return true;
  }

  return false;
}

bool CallbackManager::TryRemoveFunction(const char* name,
                                         CallbackT pCallable) {
  auto* fwd = FindCallback(name);
  if (fwd) {
    bool success = fwd->RemoveListener(pCallable);
    return success;
  }

  return false;
}

void CallbackManager::PrintCallbackDebug()
{
  /*VSPDN_CORE_INFO("--------------------\n
      ------CALLBACKS-----\n
      ")
  Msg("--------------------\n");
  Msg("------CALLBACKS-----\n");
  Msg("--------------------\n");
  for (auto it : m_managed) {
    Msg("%s (%u)\n", it->GetName().c_str(), 1);
  }
  Msg("--------------------\n");*/
}
}  // namespace vspdotnet