#pragma once

#include <raylib.h>
#include <spdlog/spdlog.h>

namespace fc::Logging
{

static void RaylibLog(int type, const char* text, va_list args)
{
    char buffer[2048];
    vsnprintf(buffer, sizeof(buffer), text, args);

    // Forward to spdlog with appropriate level
    switch (type)
    {
        case LOG_TRACE:
            spdlog::trace("{}", buffer);
            break;
        case LOG_DEBUG:
            spdlog::debug("{}", buffer);
            break;
        case LOG_INFO:
            spdlog::info("{}", buffer);
            break;
        case LOG_WARNING:
            spdlog::warn("{}", buffer);
            break;
        case LOG_ERROR:
            spdlog::error("{}", buffer);
            break;
        case LOG_FATAL:
            spdlog::critical("{}", buffer);
            break;
        default:
            break;
    }
}

inline void Initialise()
{
    spdlog::set_level(spdlog::level::debug);
    SetTraceLogCallback(RaylibLog);
}

} // namespace fc::Logging
