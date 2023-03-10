#pragma once
#include "spdlog/spdlog.h"
#include <memory>



class Logger
{
public:
  static void Init();

  inline static std::shared_ptr<spdlog::logger>& GetClientLogger() {return s_ClientLogger;}

private:
  static std::shared_ptr<spdlog::logger> s_ClientLogger;
};
#define ENGINE_TRACE(...)   ::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define ENGINE_INFO(...)    ::Logger::GetClientLogger()->info(__VA_ARGS__)
#define ENGINE_WARN(...)    ::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define ENGINE_ERROR(...)   ::Logger::GetClientLogger()->error(__VA_ARGS__)
#define ENGINE_FATAL(...)   ::Logger::GetClientLogger()->critical(__VA_ARGS__)

#define ENGINE_PRINT(x) std::cout<<x<<std::endl;
