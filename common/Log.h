
#include <array>
#include <format>
#include <iostream>
#include <source_location>

struct LogImpl
{
    enum Level
    {
        Error,
        Info,
        Debug,

        LevelCount
    };

    static constexpr std::array<const char*, LevelCount> logFormats =
        {"ERR: {}:{}: {} \n",
         "INF: {}:{}: {} \n",
         "DBG: {}:{}: {} \n"};

    template <Level level>
    static std::string buildLog(const std::string& msg, const std::source_location& location)
    {
        static_assert(logFormats[level], "Level format is not set in LogImpl::logFormats");

        return std::format(logFormats[level], location.file_name(), location.line(), msg);
    }
};

template <typename... Args>
struct LOG_ERROR
{
    LOG_ERROR(std::format_string<Args...> fmt,
              Args&&... args,
              const std::source_location location = std::source_location::current())
    {
        const auto& msg = std::format(fmt, std::forward<Args>(args)...);
        std::cerr << LogImpl::buildLog<LogImpl::Error>(msg, location);
    }
};

template <typename... Args>
LOG_ERROR(std::format_string<Args...> fmt, Args&&... args) -> LOG_ERROR<Args...>;

template <typename... Args>
struct LOG_INFO
{
    LOG_INFO(std::format_string<Args...> fmt,
             Args&&... args,
             const std::source_location location = std::source_location::current())
    {
        const auto& msg = std::format(fmt, std::forward<Args>(args)...);
        std::cerr << LogImpl::buildLog<LogImpl::Info>(msg, location);
    }
};

template <typename... Args>
LOG_INFO(std::format_string<Args...> fmt, Args&&... args) -> LOG_INFO<Args...>;

#ifdef DEBUG

template <typename... Args>
struct LOG_DEBUG
{
    LOG_DEBUG(std::format_string<Args...> fmt,
              Args&&... args,
              const std::source_location location = std::source_location::current())
    {
        const auto& msg = std::format(fmt, std::forward<Args>(args)...);
        std::cerr << LogImpl::buildLog<LogImpl::Debug>(msg, location);
    }
};

template <typename... Args>
LOG_DEBUG(std::format_string<Args...> fmt, Args&&... args) -> LOG_DEBUG<Args...>;

#else
#define LOG_DEBUG(...)
#endif
