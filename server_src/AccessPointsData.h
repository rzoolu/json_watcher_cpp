#pragma once

#include <AccessPointsDataI.h>

class AccessPointsData : public AccessPointsDataI
{
public:
    AccessPointsData() = default;
    ~AccessPointsData() override = default;

    const AccessPointMap_t& getCurrentAPs() const override;
    ChangeList_t update(const AccessPointMap_t& newData) override;

private:
    void findNewOrModifiedAPs(const AccessPointMap_t& newData, ChangeList_t& changeList);
    void findRemovedAPs(const AccessPointMap_t& newData, ChangeList_t& changeList);

    void newApChange(const AccessPoint& newAp, ChangeList_t& changeList);
    void removedApChange(const AccessPoint& oldAp, ChangeList_t& changeList);
    void apParamsChanged(const AccessPoint& oldAp,
                         const AccessPoint& newAp,
                         ChangeList_t& changeList);

private:
    AccessPointMap_t m_currentAPs;
};
