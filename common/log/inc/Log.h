#include <array>
#include <cassert>
#include <format>
#include <iostream>
#include <source_location>

enum LogLevel
{
    LOGGING_DISABLED = -1,

    ERROR = 0,
    INFO = 1,
    DEBUG = 2,

    LogLevelCount
};

// Simple logger based on std::format. Every message is prepended with log severity,
// file name and line number.
// Usage:
//          // set maximum logging level
//          Log::setLogLevel(DEBUG);
//          // disable logging
//          Log::setLogLevel(LOGGING_DISABLED);
//          // Log single entry at DEBUG level.
//          LOG(DEBUG, "Name: {} Height: {}", name, height);
//

namespace Log
{

struct LogImpl
{
    static constexpr std::array<const char*, LogLevelCount> logFormats =
        {"ERR: {}:{}: {} \n",
         "INF: {}:{}: {} \n",
         "DBG: {}:{}: {} \n"};

    static inline LogLevel currentLevel = INFO;

    static void log(LogLevel level, const std::string& msg, const std::source_location& location)
    {
        assert(level > LOGGING_DISABLED && level < LogLevelCount);

        const auto* fileName = location.file_name();
        const auto line = location.line();

        std::clog << std::vformat(logFormats[level],
                                  std::make_format_args(fileName, line, msg));
    }
};

inline void setLogLevel(LogLevel level)
{
    if (level < LogLevelCount)
    {
        Log::LogImpl::currentLevel = level;
    }
}

} // namespace Log

template <typename... Args>
struct LOG
{
    LOG(LogLevel level,
        std::format_string<Args...> fmt,
        Args&&... args,
        const std::source_location& location = std::source_location::current())
    {
        if (Log::LogImpl::currentLevel >= level)
        {
            Log::LogImpl::log(level, std::format(fmt, std::forward<Args>(args)...), location);
        }
    }
};

template <typename... Args>
LOG(LogLevel level, std::format_string<Args...> fmt, Args&&... args) -> LOG<Args...>;
