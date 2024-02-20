#pragma once

#include <JsonParserI.h>

#include <gmock/gmock.h>

class MockJsonParser : public JsonParserI
{
public:
    MOCK_METHOD(std::optional<AccessPointMap_t>, parseFromStream, (std::istream & stream), (override));
    MOCK_METHOD(std::optional<AccessPointMap_t>, parseFromFile, (const std::filesystem::path&), (override));
};