#include <array>
#include <format>
#include <iostream>
#include <source_location>

enum LogLevel
{
    ERROR,
    INFO,
    DEBUG,

    LogLevelCount
};

struct LogImpl
{
    static constexpr std::array<const char*, LogLevelCount> logFormats =
        {"ERR: {}:{}: {} \n",
         "INF: {}:{}: {} \n",
         "DBG: {}:{}: {} \n"};

    static inline LogLevel currentLevel = INFO;

    static void log(LogLevel level, const std::string& msg, const std::source_location& location)
    {
        std::cerr << std::vformat(logFormats[level],
                                  std::make_format_args(location.file_name(), location.line(), msg));
    }
};

inline void setLogLevel(LogLevel level)
{
    if (level < LogLevelCount)
    {
        LogImpl::currentLevel = level;
    }
}

template <typename... Args>
struct LOG
{
    LOG(LogLevel level,
        std::format_string<Args...> fmt,
        Args&&... args,
        const std::source_location location = std::source_location::current())
    {
        if (LogImpl::currentLevel >= level)
        {
            LogImpl::log(level, std::format(fmt, std::forward<Args>(args)...), location);
        }
    }
};

template <typename... Args>
LOG(LogLevel level, std::format_string<Args...> fmt, Args&&... args) -> LOG<Args...>;
