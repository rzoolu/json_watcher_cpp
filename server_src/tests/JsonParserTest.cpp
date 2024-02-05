#include <JsonParserI.h>

#include <gtest/gtest.h>

TEST(JsonParserTest, basicParse)
{
    const char* raw_json = R"(
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

    auto jsonParser = JsonParserI::create();

    std::stringstream istream(raw_json);

    auto apMap = jsonParser->parseFromStream(istream);

    ASSERT_TRUE(apMap);

    for (const auto& ap : *apMap)
    {
        std::cout << "ssid: " << ap.second.SSID << ", "
                  << "snr: " << ap.second.SNR << ", "
                  << "channel: " << ap.second.channel << ", "
                  << std::endl;
    }
}

TEST(JsonParserTest, parseFromFile)
{
    auto jsonParser = JsonParserI::create();

    auto apMap = jsonParser->parseFromFile("../../sample_data/access_points.json");

    ASSERT_TRUE(apMap);

    for (const auto& ap : *apMap)
    {
        std::cout << "ssid: " << ap.second.SSID << ", "
                  << "snr: " << ap.second.SNR << ", "
                  << "channel: " << ap.second.channel << ", "
                  << std::endl;
    }
}
