#ifndef TESTING_SCRIPT_ENGINE_H
#define TESTING_SCRIPT_ENGINE_H

#include <cstdint>
#include <stdexcept>
#include <functional>

#include "tl/optional.hpp"

namespace vspdotnet {

inline uint32_t HashString(const char *string) {
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
        }

        inline void PushString(const char* value) {
          auto functionData = (uint64_t *)m_argumentBuffer;
          
          *reinterpret_cast<const char**>(&functionData[m_numArguments]) = value;         
        }

        template<typename T>
        inline void SetResult(const T &value) {
          auto functionData = (uint64_t *) m_argumentBuffer;

          if (sizeof(T) < ArgumentSize) {
            *reinterpret_cast<uint64_t *>(&functionData[0]) = 0;
          }

          *reinterpret_cast<T *>(&functionData[0]) = value;

          m_numResults = 1;
          m_numArguments = 0;
        }

        template<typename T>
        inline T GetResult() {
          auto functionData = (uint64_t *) m_argumentBuffer;

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
        }

      
      inline ScriptContextRaw() {
          m_numResults = 0;
          m_numArguments = 0;
        }
    };


    /*using TNativeHandler = std::function<void(ScriptContext&)>;*/

    /*template <typename T>
    using TypedTNativeHandler = std::function<T(ScriptContext &)>;
    */


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

        static void RegisterNativeHandlerByStr(const char *nativeName,
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

          RegisterNativeHandlerInt(HashString(nativeName), lambda);
        }

        template <>
        static void RegisterNativeHandler(const char *nativeName,
                                          TypedTNativeHandler<void> function) {
          RegisterNativeHandlerInt(HashString(nativeName), function);
        }

        static void InvokeNative(vspdotnet::fxNativeContext& context);

    private:
     static fxNativeContext* m_global_context_;
     static ScriptContextRaw m_context_;
    public:
     static ScriptContextRaw &GlobalContext() { return m_context_; }
     static fxNativeContext& GlobalContextStruct() { return *m_global_context_; }
     static void SetGlobalContext(fxNativeContext& context)
     {
       m_global_context_ = &context;
       m_context_ = ScriptContextRaw(*m_global_context_);
     }
    };

/*class PluginFunction {
     public:
      PluginFunction(void *function_ptr)
          : m_function_ptr_(function_ptr), m_root_context_(fxNativeContext{}) {
        m_script_context_raw_ = ScriptContextRaw(m_root_context_);
      }

     public:
      void Invoke();
      template <typename T>
      void Push(T value) {
        m_script_context_raw_.Push(value);
      }
      ScriptContextRaw ScriptContext() const {
        return ScriptContextRaw();
        return m_script_context_raw_;
      }
      fxNativeContext& ScriptContextStruct() const {
        return ScriptEngine::GlobalContextStruct();
        //return m_root_context_;
      }

      friend bool operator==(const PluginFunction& lhs,
          const PluginFunction& rhs)
      {
        return lhs.m_function_ptr_ == rhs.m_function_ptr_;
      }

      friend bool operator!=(const PluginFunction& lhs,
          const PluginFunction& rhs)
      {
        return !(lhs == rhs);
      }

    private:
      void Reset();
      void *m_function_ptr_;
      ScriptContextRaw m_script_context_raw_;
      fxNativeContext m_root_context_;
    };*/
    }

#endif //TESTING_SCRIPT_ENGINE_H
