/*
 *  This file is part of VSP.NET.
 *  VSP.NET is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  VSP.NET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with VSP.NET.  If not, see <https://www.gnu.org/licenses/>. *
 */

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