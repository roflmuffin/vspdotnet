#pragma once

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace vspdotnet {
  class Log {
  public:
    static void Init();
    static void Close();

    static std::shared_ptr<spdlog::logger>& GetCoreLogger() {return m_core_logger; }

  private:
    static std::shared_ptr<spdlog::logger> m_core_logger;
  };
}

#define VSPDN_CORE_TRACE(...) ::vspdotnet::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define VSPDN_CORE_DEBUG(...)_ ::vspdotnet::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define VSPDN_CORE_INFO(...) ::vspdotnet::Log::GetCoreLogger()->info(__VA_ARGS__)
#define VSPDN_CORE_WARN(...) ::vspdotnet::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define VSPDN_CORE_ERROR(...) ::vspdotnet::Log::GetCoreLogger()->error(__VA_ARGS__)
#define VSPDN_CORE_CRITICAL(...) ::vspdotnet::Log::GetCoreLogger()->critical(__VA_ARGS__)