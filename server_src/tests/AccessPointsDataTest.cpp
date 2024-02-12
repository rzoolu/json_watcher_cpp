#include <AccessPointsDataI.h>

#include <gtest/gtest.h>

#include <algorithm>

namespace
{

constexpr AccessPoint AP1{"my_ap1", 1, 1};
constexpr AccessPoint AP2{"my_ap2", 2, 2};

const AccessPointMap_t oneItemMap{{AP1.SSID, AP1}};
const AccessPointMap_t twoItemsMap{{AP1.SSID, AP1},
                                   {AP2.SSID, AP2}};

} // namespace

TEST(AccessPointsDataTest, isEmtpyByDefault)
{
    const auto apData = AccessPointsDataI::create();
    const auto currentAPsMap = apData->getCurrentAPs();

    ASSERT_EQ(currentAPsMap.size(), 0u);
}

TEST(AccessPointsDataTest, properDataIsSet)
{
    auto apData = AccessPointsDataI::create();

    apData->update(oneItemMap);

    const auto currentAPsMap = apData->getCurrentAPs();
    ASSERT_EQ(oneItemMap, currentAPsMap);
}

TEST(AccessPointsDataTest, changeListIsEmptyWhenNoChanges)
{
    auto apData = AccessPointsDataI::create();

    apData->update(twoItemsMap);
    const ChangeList_t changeList = apData->update(twoItemsMap);

    ASSERT_TRUE(changeList.empty());

    const auto currentAPsMap = apData->getCurrentAPs();
    ASSERT_EQ(twoItemsMap, currentAPsMap);
}

TEST(AccessPointsDataTest, APAdditionIsDetected)
{
    auto apData = AccessPointsDataI::create();

    apData->update(oneItemMap);
    const ChangeList_t changeList = apData->update(twoItemsMap);

    ASSERT_EQ(changeList.size(), 1u);
    ASSERT_EQ(changeList[0].changeType, APDataChange::NewAP);
    ASSERT_EQ(changeList[0].newAP, AP2);

    const auto currentAPsMap = apData->getCurrentAPs();
    ASSERT_EQ(twoItemsMap, currentAPsMap);
}

TEST(AccessPointsDataTest, TwoAPAdditionIsDetected)
{
    auto apData = AccessPointsDataI::create();

    const ChangeList_t changeList = apData->update(twoItemsMap);

    ASSERT_EQ(changeList.size(), 2u);
    ASSERT_EQ(changeList[0].changeType, APDataChange::NewAP);
    ASSERT_EQ(changeList[1].changeType, APDataChange::NewAP);
}

TEST(AccessPointsDataTest, APRemovalIsDetected)
{
    auto apData = AccessPointsDataI::create();

    apData->update(twoItemsMap);
    const ChangeList_t changeList = apData->update(oneItemMap);

    ASSERT_EQ(changeList.size(), 1u);
    ASSERT_EQ(changeList[0].changeType, APDataChange::RemovedAP);
    ASSERT_EQ(changeList[0].oldAP, AP2);

    const auto currentAPsMap = apData->getCurrentAPs();
    ASSERT_EQ(oneItemMap, currentAPsMap);
}

TEST(AccessPointsDataTest, APModificationIsDetected)
{
    auto apData = AccessPointsDataI::create();

    apData->update(twoItemsMap);

    auto moddifiedAP2Map = twoItemsMap;

    moddifiedAP2Map[AP2.SSID].channel++;
    moddifiedAP2Map[AP2.SSID].SNR++;

    const ChangeList_t changeList = apData->update(moddifiedAP2Map);

    ASSERT_EQ(changeList.size(), 1u);
    ASSERT_EQ(changeList[0].changeType, APDataChange::APParamsChanged);
    ASSERT_EQ(changeList[0].oldAP, AP2);
    ASSERT_EQ(changeList[0].newAP, moddifiedAP2Map[AP2.SSID]);

    ASSERT_TRUE(std::find(changeList[0].changedParams.begin(),
                          changeList[0].changedParams.end(),
                          APDataChange::channnel) != changeList[0].changedParams.end());

    ASSERT_TRUE(std::find(changeList[0].changedParams.begin(),
                          changeList[0].changedParams.end(),
                          APDataChange::SNR) != changeList[0].changedParams.end());
}
