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

#include "core/log.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace vspdotnet {
std::shared_ptr<spdlog::logger> Log::m_core_logger;

void Log::Init() {
  std::vector<spdlog::sink_ptr> logSinks;
  logSinks.emplace_back(
      std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
  logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      "vspdotnet.log", true));

  logSinks[0]->set_pattern("%^[%T] %n: %v%$");
  logSinks[1]->set_pattern("[%T] [%l] %n: %v");

  m_core_logger = std::make_shared<spdlog::logger>(
      "Source.NET", begin(logSinks), end(logSinks));
  spdlog::register_logger(m_core_logger);
  m_core_logger->set_level(spdlog::level::info);
  m_core_logger->flush_on(spdlog::level::info);
}

void Log::Close() {
  spdlog::drop("Source.NET");
  m_core_logger = nullptr;
}
}  // namespace vspdotnet
