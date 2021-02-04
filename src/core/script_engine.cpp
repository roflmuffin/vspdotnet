#include "core/script_engine.h"

#include <stack>
#include <unordered_map>

#include "core/log.h"
#include "core/utils.h"

static std::unordered_map<uint64_t, vspdotnet::TNativeHandler> g_registeredHandlers;


namespace vspdotnet {

    std::stack<std::string> errors;

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
        errors.push(error_string);

        const char* ptr = errors.top().c_str();
        this->SetResult(ptr);
        *this->m_has_error = 1;
    }

    void ScriptContext::Reset()
    {
      if (*m_has_error)
      {
        errors.pop();
      }

      m_numResults = 0;
      m_numArguments = 0;
      *m_has_error = 0;

      for (int i = 0; i < 32;i++)
      {
        m_native_context->arguments[i] = 0;
      }

      m_native_context->result = 0;
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
      auto it = g_registeredHandlers.find(hash_string(identifier.c_str()));

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

    void ScriptEngine::InvokeNative(vspdotnet::fxNativeContext& context) {
      if (context.nativeIdentifier == 0) return;


      auto nativeHandler = vspdotnet::ScriptEngine::GetNativeHandler(context.nativeIdentifier);

      if (nativeHandler) {
        vspdotnet::ScriptContextRaw scriptContext(context);

        (*nativeHandler)(scriptContext);
      } else
      {
        VSPDN_CORE_WARN("Native Handler was requested but not found: {0:x}",
                        context.nativeIdentifier);
        assert(false);
      }     
      
    }

    ScriptContextRaw ScriptEngine::m_context;

  }
