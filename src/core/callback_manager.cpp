#include "callback_manager.h"
#include "core/log.h"

namespace vspdotnet {

CCallback::CCallback(const char* name) : m_root_context_(fxNativeContext{}) {
  m_script_context_raw_ = ScriptContextRaw(m_root_context_);
  m_name_ = std::string(name);
}

void CCallback::AddListener(CallbackT plugin_function) {
  m_functions_.push_back(plugin_function);
}


bool CCallback::RemoveListener(CallbackT plugin_function) {
  bool success;
  auto it = std::remove_if(m_functions_.begin(), m_functions_.end(),
      [plugin_function](void* i)
      {
        return plugin_function == i;
      });

  if ((success = it != m_functions_.end()))
    m_functions_.erase(it, m_functions_.end());

  return success;
}

void CCallback::Execute(bool resetContext)
{
  for (auto method_to_call : m_functions_) {
    if (method_to_call) {
      method_to_call(&ScriptContextStruct());
    }
  }

  if (resetContext) {
    ResetContext();
  }
}

void CCallback::ResetContext()
{
  ScriptContext().Reset();
}

CCallbackManager::CCallbackManager() {}


CCallback* CCallbackManager::CreateCallback(const char* name)
{
  auto* callback = new CCallback(name);
  m_managed_.push_back(callback);

  return callback;
}

CCallback* CCallbackManager::FindCallback(const char* name) {
  for (auto it = m_managed_.begin(); it != m_managed_.end(); ++it) {
    CCallback* marshal = *it;
    if (strcmp(marshal->GetName().c_str(), name) == 0) {
      return marshal;
    }
  }

  return nullptr;
}

void CCallbackManager::ReleaseCallback(CCallback* callback) {
  bool success;
  auto it = std::remove_if(
      m_managed_.begin(), m_managed_.end(),
                           [callback](CCallback* i) { return callback == i; });

  if ((success = it != m_managed_.end()))
    m_managed_.erase(it, m_managed_.end());
  delete callback;
}

bool CCallbackManager::TryAddFunction(const char* name, CallbackT pCallable) {
  auto* fwd = FindCallback(name);
  if (fwd) {
    fwd->AddListener(pCallable);
    return true;
  }

  return false;
}

bool CCallbackManager::TryRemoveFunction(const char* name,
                                         CallbackT pCallable) {
  auto* fwd = FindCallback(name);
  if (fwd) {
    bool success = fwd->RemoveListener(pCallable);
    return success;
  }

  return false;
}

void CCallbackManager::PrintCallbackDebug()
{
  /*VSPDN_CORE_INFO("--------------------\n
      ------CALLBACKS-----\n
      ")
  Msg("--------------------\n");
  Msg("------CALLBACKS-----\n");
  Msg("--------------------\n");
  for (auto it : m_managed_) {
    Msg("%s (%u)\n", it->GetName().c_str(), 1);
  }
  Msg("--------------------\n");*/
}
}  // namespace vspdotnet