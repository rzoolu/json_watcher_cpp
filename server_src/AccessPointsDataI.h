#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using Ssid_t = std::string;

struct AccessPoint
{
    Ssid_t SSID;
    std::uint16_t SNR;
    std::uint16_t channel;

    friend bool operator==(const AccessPoint& lhs, const AccessPoint& rhs) = default;
};

using AccessPointMap_t = std::unordered_map<Ssid_t, AccessPoint>;

struct APDataChange
{
    enum Type
    {
        NewAP,
        RemovedAP,
        APParamsChanged
    };

    enum Param
    {
        None,
        SNR,
        channnel,
    };

    Type changeType;
    AccessPoint oldAP;
    AccessPoint newAP;
    std::vector<Param> changedParams;
};

using ChangeList_t = std::vector<APDataChange>;

class AccessPointsDataI
{
public:
    using AccessPointsDataFactory_t =
        std::function<std::unique_ptr<AccessPointsDataI>()>;

    static AccessPointsDataFactory_t create;

    ~AccessPointsDataI() = default;

    virtual ChangeList_t update(const AccessPointMap_t& newData) = 0;
    virtual const AccessPointMap_t& getCurrentAPs() const = 0;
};
