#pragma once

#include <vector>

#include "core/globals.h"
#include "core/global_listener.h"
#include "core/script_engine.h"

namespace vspdotnet {

class ScriptCallback
{
public:
  ScriptCallback(const char* name);
  ~ScriptCallback();
 void AddListener(CallbackT plugin_function);
  bool RemoveListener(CallbackT plugin_function);
  std::string GetName() { return m_name; }
  unsigned int GetFunctionCount() { return m_functions.size(); }
  void Execute(bool resetContext = true);
  void ResetContext();
  ScriptContextRaw& ScriptContext()
  { return m_script_context_raw;
  }
  fxNativeContext& ScriptContextStruct() {
    return m_root_context;
  }

private:
  std::vector<CallbackT> m_functions;
  std::string m_name;
  ScriptContextRaw m_script_context_raw;
  fxNativeContext m_root_context;
};

class CallbackManager : public GlobalClass {
 public:
  CallbackManager();

 public:
  ScriptCallback* CreateCallback(const char* name);
  ScriptCallback* FindCallback(const char* name);
  void ReleaseCallback(ScriptCallback* callback);
  bool TryAddFunction(const char* name, CallbackT pCallable);
  bool TryRemoveFunction(const char* name, CallbackT pCallable);
  void PrintCallbackDebug();

 private:
  std::vector<ScriptCallback*> m_managed;
};

}  // namespace vspdotnet