
#include "ClientApp.h"

#include <Log.h>
#include <ProtoBufGuard.h>

#include <algorithm>
#include <array>
#include <string_view>
#include <unordered_map>

namespace
{
using OptionValMap =
    std::unordered_map<std::string_view, std::string_view>;

constexpr auto OPTION_DEBUG = "-d";
constexpr auto OPTION_HELP = "-h";

constexpr auto ALL_KNOWN_OPTIONS =
    std::to_array<std::string_view>({OPTION_DEBUG,
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
        const std::string_view option(argv[i]);

        if (isValidOption(option))
        {
            std::string_view value;

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
} // namespace

int main(const int argc, char* argv[])
{
    const ProtoBufGuard pbGuard;

    const auto options = createOptionMap(argc, argv);

    if (options.contains(OPTION_DEBUG))
    {
        setLogLevel(DEBUG);
    }
    else
    {
        // disable all non-critical standard output logging
        // to allow client's text UI.
        setLogLevel(ERROR);
    }

    ClientApp app;
    LOG(DEBUG, "Client started.");
    app.run();

    return 0;
}
