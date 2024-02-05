#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

using Ssid_t = std::string;

struct AccessPoint
{
    Ssid_t SSID;
    std::uint16_t SNR;
    std::uint16_t channel;
};

using AccessPointMap_t = std::unordered_map<Ssid_t, AccessPoint>;