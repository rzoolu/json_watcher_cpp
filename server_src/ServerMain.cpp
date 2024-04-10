
#include "ServerApp.h"

#include <Log.h>
#include <ProtoBufGuard.h>

#include <algorithm>
#include <array>
#include <format>
#include <iostream>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace
{
using OptionValMap =
    std::unordered_map<std::string_view, std::string_view>;

constexpr auto OPTION_DEBUG = "-d";
constexpr auto OPTION_FILE_PATH = "-f";
constexpr auto OPTION_HELP = "-h";

constexpr auto ALL_KNOWN_OPTIONS =
    std::to_array<std::string_view>({OPTION_DEBUG,
                                     OPTION_FILE_PATH,
                                     OPTION_HELP});

constexpr bool isValidOption(std::string_view option)
{
    return ALL_KNOWN_OPTIONS.end() !=
           std::find(ALL_KNOWN_OPTIONS.begin(), ALL_KNOWN_OPTIONS.end(), option);
}

OptionValMap createOptionMap(const int argc, char* argv[])
{
    OptionValMap options;

    for (auto i = 1; i < argc; ++i)
    {
        std::string_view option(argv[i]);
        std::string_view value;

        if (isValidOption(option))
        {
            // if there's next arg which is not option, use it as option value
            if (i + 1 < argc && !isValidOption(argv[i + 1]))
            {
                value = argv[i + 1];
                ++i;
            }

            options.emplace(option, value);
        }
    }

    return options;
}

void debugArgv(const int argc, char* argv[])
{
    LOG(DEBUG, "Server started, argc = {}", argc);

    for (auto i = 0; i < argc; ++i)
    {
        LOG(DEBUG, "argv[{}] = {},", i, argv[i]);
    }
}

void printUsageInfo(std::string_view progName)
{
    constexpr auto info = R"(

Usage:
 {} -f PATH [options]

Options:
 -f PATH             AP file to be watched.
 -d                  Enable debug logs.
 -h                  Print this help and exit.

)";

    std::cout << std::format(info, progName);
}

}; // namespace

int main(int argc, char* argv[])
{
    const ProtoBufGuard pbGuard;

    const auto options = createOptionMap(argc, argv);

    if (options.contains(OPTION_DEBUG))
    {
        setLogLevel(DEBUG);
        debugArgv(argc, argv);
    }

    if (options.contains(OPTION_HELP))
    {
        printUsageInfo(argv[0]);
        return 0;
    }

    if (!options.contains(OPTION_FILE_PATH) ||
        options.at(OPTION_FILE_PATH).empty())
    {
        printUsageInfo(argv[0]);
        return -1;
    }

    ServerApp app(options.at(OPTION_FILE_PATH));
    LOG(DEBUG, "App started.");
    app.run();

    return 0;
}
