#include <AccessPointsData.h>

AccessPointsDataI::AccessPointsDataFactory_t AccessPointsDataI::create = []()
{
    return std::make_unique<AccessPointsData>();
};

const AccessPointMap_t& AccessPointsData::getCurrentAPs() const
{
    return m_currentAPs;
}

ChangeList_t AccessPointsData::update(const AccessPointMap_t& newData)
{
    ChangeList_t changeList;

    findNewOrModifiedAPs(newData, changeList);
    findRemovedAPs(newData, changeList);

    return changeList;
}

void AccessPointsData::findNewOrModifiedAPs(const AccessPointMap_t& newData, ChangeList_t& changeList)
{
    for (const auto& [ssid, newAP] : newData)
    {
        auto currentAPIt = m_currentAPs.find(newAP.SSID);

        if (currentAPIt == m_currentAPs.end())
        {
            newApChange(newAP, changeList);
        }
        else if (currentAPIt->second != newAP)
        {
            apParamsChanged(currentAPIt->second, newAP, changeList);
        }
    }
}

void AccessPointsData::findRemovedAPs(const AccessPointMap_t& newData, ChangeList_t& changeList)
{
    for (const auto& [ssid, currentAP] : m_currentAPs)
    {
        if (!newData.contains(currentAP.SSID))
        {
            removedApChange(currentAP, changeList);
        }
    }
}

void AccessPointsData::newApChange(const AccessPoint& newAp, ChangeList_t& changeList)
{
    changeList.emplace_back(
        APDataChange::NewAP, AccessPoint{}, newAp, std::vector<APDataChange::Param>{});
}

void AccessPointsData::removedApChange(const AccessPoint& oldAp, ChangeList_t& changeList)
{
    changeList.emplace_back(
        APDataChange::RemovedAP, oldAp, AccessPoint{}, std::vector<APDataChange::Param>{});
}

void AccessPointsData::apParamsChanged(const AccessPoint& oldAp,
                                       const AccessPoint& newAp,
                                       ChangeList_t& changeList)
{

    std::vector<APDataChange::Param> changedParams;

    if (oldAp.SNR != newAp.SNR)
    {
        changedParams.push_back(APDataChange::SNR);
    }

    if (oldAp.channel != newAp.channel)
    {
        changedParams.push_back(APDataChange::channnel);
    }

    changeList.emplace_back(
        APDataChange::APParamsChanged, oldAp, newAp, std::move(changedParams));
}
