#include <vector>

#include "core/globals.h"
#include "core/script_engine.h"

namespace vspdotnet {

class CCallback
{
public:
  CCallback(const char* name);
 void AddListener(CallbackT plugin_function);
  bool RemoveListener(CallbackT plugin_function);
  std::string GetName() { return m_name_; }
  unsigned int GetFunctionCount() { return m_functions_.size(); }
  void Execute(bool resetContext = true);
  void ResetContext();
  ScriptContextRaw& ScriptContext()
  { return m_script_context_raw_;
  }
  fxNativeContext& ScriptContextStruct() {
    return m_root_context_;
  }

private:
  std::vector<CallbackT> m_functions_;
  std::string m_name_;
  ScriptContextRaw m_script_context_raw_;
  fxNativeContext m_root_context_;
};

class CCallbackManager : public GlobalClass {
 public:
  CCallbackManager();

 public:
  CCallback* CreateCallback(const char* name);
  CCallback* FindCallback(const char* name);
  void ReleaseCallback(CCallback* callback);
  bool TryAddFunction(const char* name, CallbackT pCallable);
  bool TryRemoveFunction(const char* name, CallbackT pCallable);
  void PrintCallbackDebug();

 private:
  std::vector<CCallback*> m_managed_;
};

}  // namespace vspdotnet