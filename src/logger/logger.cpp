#include "logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>


std::shared_ptr<spdlog::logger> Logger::s_ClientLogger;
void Logger::Init()
{
  spdlog::set_pattern("%^[%T] %n: %v%$");
  s_ClientLogger = spdlog::stdout_color_mt("Engine");
  s_ClientLogger->set_level(spdlog::level::trace);
}
