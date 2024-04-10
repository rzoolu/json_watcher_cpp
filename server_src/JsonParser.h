#pragma once

#include "JsonParserI.h"

#include <nlohmann/json_fwd.hpp>

class JsonParser : public JsonParserI
{
public:
    virtual ~JsonParser() override = default;

    virtual std::optional<AccessPointMap_t> parseFromStream(std::istream& stream) override;
    virtual std::optional<AccessPointMap_t> parseFromFile(const std::filesystem::path& file) override;

private:
    bool containsMandatoryData(const nlohmann::json& data);
    AccessPointMap_t creaateAccessPointMap(const nlohmann::json& data);
};
