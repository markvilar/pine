#pragma once

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#include "Pine/Core/Base.hpp"

namespace Pine
{

class Log
{
public:
    static void Init();

    static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
    static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

private:
    static Ref<spdlog::logger> s_CoreLogger;
    static Ref<spdlog::logger> s_ClientLogger;
};

} // namespace Pine

// Core log macros.
#define PINE_CORE_TRACE(...) ::Pine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define PINE_CORE_INFO(...) ::Pine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define PINE_CORE_WARN(...) ::Pine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define PINE_CORE_ERROR(...) ::Pine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define PINE_CORE_CRITICAL(...)                                                \
    ::Pine::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros.
#define PINE_TRACE(...) ::Pine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define PINE_INFO(...) ::Pine::Log::GetClientLogger()->info(__VA_ARGS__)
#define PINE_WARN(...) ::Pine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define PINE_ERROR(...) ::Pine::Log::GetClientLogger()->error(__VA_ARGS__)
#define PINE_CRITICAL(...) ::Pine::Log::GetClientLogger()->critical(__VA_ARGS__)