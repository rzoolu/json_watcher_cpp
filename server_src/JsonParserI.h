#pragma once

#include <AccessPointsDataI.h>

#include <filesystem>
#include <functional>
#include <istream>
#include <optional>

class JsonParserI
{
public:
    using JsonParserFactory_t =
        std::function<std::unique_ptr<JsonParserI>()>;

    static JsonParserFactory_t create;

    virtual ~JsonParserI() = default;

    virtual std::optional<AccessPointMap_t> parseFromStream(std::istream& stream) = 0;
    virtual std::optional<AccessPointMap_t> parseFromFile(const std::filesystem::path& file) = 0;
};
