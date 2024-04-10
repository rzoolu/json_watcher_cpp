#include "AccessPointsData.h"

#include <Log.h>

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

    if (!changeList.empty())
    {
        LOG(DEBUG, "AccessPointsData updated.");
        m_currentAPs = newData;
    }
    else
    {
        LOG(DEBUG, "No data changes detected.");
    }

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
    LOG(DEBUG, "AccessPointsData new AP detected: {}", newAp.SSID);

    changeList.emplace_back(NewApChange{newAp});
}

void AccessPointsData::removedApChange(const AccessPoint& oldAp, ChangeList_t& changeList)
{
    LOG(DEBUG, "AccessPointsData AP removal detected: {}", oldAp.SSID);

    changeList.emplace_back(RemovedApChange{oldAp});
}

void AccessPointsData::apParamsChanged(const AccessPoint& oldAp,
                                       const AccessPoint& newAp,
                                       ChangeList_t& changeList)
{
    LOG(DEBUG, "AccessPointsData AP params change detected: {}", oldAp.SSID);

    std::vector<ModifiedApParamsChange::Param> changedParams;

    if (oldAp.SNR != newAp.SNR)
    {
        changedParams.push_back(ModifiedApParamsChange::SNR);
    }

    if (oldAp.channel != newAp.channel)
    {
        changedParams.push_back(ModifiedApParamsChange::channnel);
    }

    changeList.emplace_back(ModifiedApParamsChange{oldAp,
                                                   newAp,
                                                   std::move(changedParams)});
}
