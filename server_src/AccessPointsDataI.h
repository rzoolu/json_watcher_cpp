#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
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

struct NewApChange
{
    AccessPoint newAP;
};

struct RemovedApChange
{
    AccessPoint oldAP;
};

struct ModifiedApParamsChange
{
    enum Param
    {
        None,
        SNR,
        channnel,
    };

    AccessPoint oldAP;
    AccessPoint newAP;
    std::vector<Param> changedParams;
};

using APDataChange_t =
    std::variant<NewApChange, RemovedApChange, ModifiedApParamsChange>;

using ChangeList_t = std::vector<APDataChange_t>;

class AccessPointsDataI
{
public:
    using AccessPointsDataFactory_t =
        std::function<std::unique_ptr<AccessPointsDataI>()>;

    static AccessPointsDataFactory_t create;

    virtual ~AccessPointsDataI() = default;

    virtual const AccessPointMap_t& getCurrentAPs() const = 0;
    virtual ChangeList_t update(const AccessPointMap_t& newData) = 0;
};
