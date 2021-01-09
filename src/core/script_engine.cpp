#include "script_engine.h"

#include <unordered_map>

#include "utils.h"
#include "log.h"

static std::unordered_map<uint64_t, vspdotnet::TNativeHandler> g_registeredHandlers;

std::vector<std::string> errors;

namespace vspdotnet {
    void ScriptContext::ThrowNativeError(const char* msg, ...)
    {
        va_list arglist;
        char dest[256];
        va_start(arglist, msg);
        vsprintf(dest, msg, arglist);
        va_end(arglist);
        char buff[256];
        snprintf(buff, sizeof(buff), dest, arglist);

        auto error_string = std::string(buff);
        errors.push_back(error_string);

        const char* ptr = errors[0].c_str();
        this->SetResult(ptr);
        *this->m_has_error = 1;
    }

    void ScriptContext::Reset()
    {
      m_numResults = 0;
      m_numArguments = 0;
      *m_has_error = 0;

      for (int i = 0; i < 32;i++)
      {
        m_native_context_->arguments[i] = 0;
      }
    }

    tl::optional<TNativeHandler> ScriptEngine::GetNativeHandler(uint64_t nativeIdentifier) {
      auto it = g_registeredHandlers.find(nativeIdentifier);

      if (it != g_registeredHandlers.end())
      {
        return it->second;
      }

      return tl::optional<TNativeHandler>();
    }

    tl::optional<TNativeHandler> ScriptEngine::GetNativeHandler(std::string identifier) {
      auto it = g_registeredHandlers.find(HashString(identifier.c_str()));

      if (it != g_registeredHandlers.end())
      {
        return it->second;
      }

      return tl::optional<TNativeHandler>();
    }



    bool ScriptEngine::CallNativeHandler(uint64_t nativeIdentifier, ScriptContext &context) {
      auto h = GetNativeHandler(nativeIdentifier);
      if (h) {
        (*h)(context);

        return true;
      }

      return false;
    }

    void ScriptEngine::RegisterNativeHandlerInt(uint64_t nativeIdentifier, TNativeHandler function) {
      g_registeredHandlers[nativeIdentifier] = function;
    }

    void ScriptEngine::RegisterNativeHandlerByStr(const char* nativeName, TNativeHandler function) {
      RegisterNativeHandlerInt(HashString(nativeName), function);
    }

    void ScriptEngine::InvokeNative(vspdotnet::fxNativeContext& context) {
      if (context.nativeIdentifier == 0) return;

      if (!m_global_context_) {
        SetGlobalContext(context);
      }

      auto nativeHandler = vspdotnet::ScriptEngine::GetNativeHandler(context.nativeIdentifier);

      if (nativeHandler) {
        vspdotnet::ScriptContextRaw scriptContext(context);

        (*nativeHandler)(scriptContext);
      } else
      {
        VSPDN_CORE_WARN("Native Handler was requested but not found: {0:x}",
                        context.nativeIdentifier);  
      }     
      
    }

    fxNativeContext* ScriptEngine::m_global_context_ = nullptr;
    ScriptContextRaw ScriptEngine::m_context_;

    /*
    void PluginFunction::Invoke() {
      typedef void(__stdcall * callback_fn)();
      auto callback = static_cast<callback_fn>(m_function_ptr_);

//      callback(m_root_context_);
      callback();

      Reset();
    }

    void PluginFunction::Reset()
    {
      m_root_context_ = fxNativeContext{};
      m_script_context_raw_ = ScriptContextRaw(m_root_context_);
    }
    */

  }
