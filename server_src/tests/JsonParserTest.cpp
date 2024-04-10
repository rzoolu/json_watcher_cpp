#include <JsonParserI.h>

#include <AccessPointsDataI.h>

#include <gtest/gtest.h>
#include <memory>

class JsonParserTest : public testing::Test
{
protected:
    JsonParserTest() : m_jsonParser(JsonParserI::create()) {}

    std::unique_ptr<JsonParserI> m_jsonParser;
};

TEST_F(JsonParserTest, parseSingleAp)
{
    const char* rawJson = R"(
    {
        "access_points" : [
            {
                "ssid" : "MyAP",
                "snr" : 63,
                "channel" : 11
            }
        ]
    }
    )";

    std::stringstream istream(rawJson);

    auto apMap = m_jsonParser->parseFromStream(istream);
    ASSERT_TRUE(apMap.has_value());
    ASSERT_EQ(apMap->size(), 1u);

    const auto& ap = apMap->at("MyAP");

    ASSERT_EQ(ap.SSID, Ssid_t("MyAP"));
    ASSERT_EQ(ap.SNR, 63u);
    ASSERT_EQ(ap.channel, 11u);
}

TEST_F(JsonParserTest, SkipInvalidAp)
{
    const char* rawJson = R"(
    {
        "access_points" : [
            {
                "ssid" : "MyAP",
                "snr" : 63,
                "channel" : 11
            },
            {
                "ssid" : "MyAP2",
                "channel" : 11
            }
        ]
    }
    )";

    std::stringstream istream(rawJson);

    auto apMap = m_jsonParser->parseFromStream(istream);
    ASSERT_TRUE(apMap.has_value());
    ASSERT_EQ(apMap->size(), 1u);

    const auto& ap = apMap->at("MyAP");

    ASSERT_EQ(ap.SSID, Ssid_t("MyAP"));
    ASSERT_EQ(ap.SNR, 63u);
    ASSERT_EQ(ap.channel, 11u);
}

TEST_F(JsonParserTest, InvalidJson)
{
    const char* rawJson = R"(
    {
        "access_points" : [
    )";

    std::stringstream istream(rawJson);

    auto apMap = m_jsonParser->parseFromStream(istream);
    ASSERT_FALSE(apMap.has_value());
}

TEST_F(JsonParserTest, parseFromValidFile)
{
    constexpr auto* sampleFilePath = "./access_points.json";

    auto apMap = m_jsonParser->parseFromFile(sampleFilePath);
    ASSERT_TRUE(apMap.has_value());

    ASSERT_EQ(apMap->size(), 10u);

    const auto& ap = apMap->at("MyAP");

    ASSERT_EQ(ap.SSID, Ssid_t("MyAP"));
    ASSERT_EQ(ap.SNR, 63u);
    ASSERT_EQ(ap.channel, 11u);

    const auto& otherAp = apMap->at("HOME3");

    ASSERT_EQ(otherAp.SSID, Ssid_t("HOME3"));
    ASSERT_EQ(otherAp.SNR, 71u);
    ASSERT_EQ(otherAp.channel, 10u);
}
