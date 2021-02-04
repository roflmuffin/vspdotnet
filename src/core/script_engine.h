#pragma once

#include <cstdint>
#include <stdexcept>
#include <functional>

#include "tl/optional.hpp"


namespace vspdotnet {

inline uint32_t hash_string(const char *string) {
  unsigned long result = 5381;

  for (int i = 0; i < strlen(string); i++) {
    result = ((result << 5) + result) ^ string[i];
  }

  return result;
}

struct fxNativeContext
    {
        int numArguments;
        int numResults;
        int hasError;

        uint64_t nativeIdentifier;
        uint64_t arguments[32];
        uint64_t result;
    };

typedef void(__stdcall *CallbackT)(fxNativeContext*);

    class ScriptContext {
    public:
        enum {
            MaxArguments = 32,
            ArgumentSize = sizeof(uint64_t)
        };

    protected:
        void *m_argumentBuffer;

        int m_numArguments;
        int m_numResults;
        int *m_has_error;
        uint64_t *m_nativeIdentifier;
        void *m_result;
        

        fxNativeContext *m_native_context_;

    public:
        inline ScriptContext() = default;

    public:
        void ThrowNativeError(const char *msg, ...);
        bool HasError() { return *m_has_error == 1; }
        void Reset();

    public:
        template<typename T>
        inline const T &GetArgument(int index) {
          auto functionData = (uint64_t *) m_argumentBuffer;

          return *reinterpret_cast<T *>(&functionData[index]);
        }

        template<typename T>
        inline void SetArgument(int index, const T &value) {
          auto functionData = (uint64_t *) m_argumentBuffer;

          static_assert(sizeof(T) <= ArgumentSize, "Argument size of T");

          if (sizeof(T) < ArgumentSize) {
            *reinterpret_cast<uint64_t *>(&functionData[index]) = 0;
          }

          *reinterpret_cast<T *>(&functionData[index]) = value;
        }

        template<typename T>
        inline const T &CheckArgument(int index) {
          const auto &argument = GetArgument<T>(index);

          if (argument == T()) {
            throw std::runtime_error("Argument at index %d was null.");
          }

          return argument;
        }

        inline int GetArgumentCount() {
          return m_numArguments;
        }

        template<typename T>
        inline void Push(const T &value) {
          auto functionData = (uint64_t *) m_argumentBuffer;

          static_assert(sizeof(T) <= ArgumentSize, "Argument size of T");

          if (sizeof(T) < ArgumentSize) {
            *reinterpret_cast<uint64_t *>(&functionData[m_numArguments]) = 0;
          }

          *reinterpret_cast<T *>(&functionData[m_numArguments]) = value;
          m_numArguments++;
          m_native_context_->numArguments++;
        }

        inline void PushString(const char* value) {
          auto functionData = (uint64_t *)m_argumentBuffer;
          
          *reinterpret_cast<const char**>(&functionData[m_numArguments]) = value;         
        }

        template<typename T>
        inline void SetResult(const T &value) {
          auto functionData = (uint64_t *) m_result;

          if (sizeof(T) < ArgumentSize) {
            *reinterpret_cast<uint64_t *>(&functionData[0]) = 0;
          }

          *reinterpret_cast<T *>(&functionData[0]) = value;

          m_numResults = 1;
          m_numArguments = 0;
        }

        template <typename T>
        inline T GetResult() {
          auto functionData = (uint64_t *)m_result;

          return *reinterpret_cast<T *>(functionData);
        }

        inline void *GetArgumentBuffer() {
          return m_argumentBuffer;
        }

        inline int GetNumArguments()
        { return m_native_context_->numArguments;
        }
    };

    class ScriptContextRaw : public ScriptContext
    {
    public:
        inline ScriptContextRaw(fxNativeContext& context)
        {
          m_argumentBuffer = context.arguments;

          m_numArguments = context.numArguments;
          m_numResults = 0;
          m_has_error = &context.hasError;
          m_nativeIdentifier = &context.nativeIdentifier;
          m_native_context_ = &context;
          m_result = &context.result;
        }

      
      inline ScriptContextRaw() {
          m_numResults = 0;
          m_numArguments = 0;
        }
    };

    using TNativeHandler = std::function<void(ScriptContext&)>;

    template <typename T>
    using TypedTNativeHandler =  T(ScriptContext&);

    class ScriptEngine {
    public:
        static tl::optional<TNativeHandler> GetNativeHandler(uint64_t nativeIdentifier);
     static tl::optional<TNativeHandler> GetNativeHandler(
         std::string name);

        static bool CallNativeHandler(uint64_t nativeIdentifier, ScriptContext &context);

        static void RegisterNativeHandlerInt(uint64_t nativeIdentifier,
                                             TNativeHandler function);


        template <typename T>
        static void RegisterNativeHandler(
          const char* nativeName, TypedTNativeHandler<T> function) {

          auto lambda = [=](vspdotnet::ScriptContext &context) {
            auto value = function(context);
            if (!context.HasError()) {
              context.SetResult(value);
            }
          };

          RegisterNativeHandlerInt(hash_string(nativeName), lambda);
        }
      
        template <>
        static void RegisterNativeHandler(const char* nativeName,
                                          TypedTNativeHandler<void> function)
        {
          RegisterNativeHandlerInt(hash_string(nativeName), function);
        }

        static void InvokeNative(vspdotnet::fxNativeContext& context);

    private:
     static ScriptContextRaw m_context;
    };
    }
