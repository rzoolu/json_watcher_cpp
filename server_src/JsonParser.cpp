#include <JsonParser.h>
#include <Log.h>

#include <algorithm>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

JsonParserI::JsonParserFactory_t JsonParserI::create = []()
{
    return std::make_unique<JsonParser>();
};

constexpr auto mandatoryAPFields = {"ssid", "snr", "channel"};

std::optional<AccessPointMap_t> JsonParser::parseFromStream(std::istream& istream)
{
    constexpr auto CALLBACK = nullptr;
    constexpr auto EXCEPTIONS = false;
    constexpr auto IGNORE_COMMENTS = true;

    json inputData = json::parse(istream, CALLBACK, EXCEPTIONS, IGNORE_COMMENTS);

    if (inputData.is_discarded())
    {
        LOG(INFO, "JSON parsing error.");
        return std::nullopt;
    }

    if (!inputData.contains("access_points") ||
        !inputData["access_points"].is_array())
    {
        LOG(ERROR, "JSON parsing, missing access_points array");
        return std::nullopt;
    }

    return creaateAccessPointMap(inputData["access_points"]);
}

std::optional<AccessPointMap_t> JsonParser::parseFromFile(const std::filesystem::path& file)
{
    std::ifstream input(file);

    if (!input.good())
    {
        LOG(ERROR, "File: {} cannot be read.", file.string());
        return std::nullopt;
    }

    LOG(DEBUG, "JSON parsing file: {}.", file.string());

    return parseFromStream(input);
}
AccessPointMap_t JsonParser::creaateAccessPointMap(const json& aps)
{
    AccessPointMap_t apMap;

    for (const auto& ap : aps)
    {
        if (containsMandatoryData(ap))
        {
            const auto ssid = ap["ssid"];

            apMap.emplace(ssid,
                          AccessPoint{ssid, ap["snr"], ap["channel"]});
        }
        else
        {
            LOG(ERROR, "JSON parsing, invalid AP entry.");
        }
    }

    LOG(DEBUG, "JSON parsing, {} AP entries found.", apMap.size());

    return apMap;
}

bool JsonParser::containsMandatoryData(const json& ap)
{
    return std::all_of(mandatoryAPFields.begin(),
                       mandatoryAPFields.end(),
                       [&](const auto& key)
                       { return ap.contains(key); });
}
