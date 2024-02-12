#include <AccessPointsDataI.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing;

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

    ASSERT_THAT(apData->getCurrentAPs(), IsEmpty());
}

TEST(AccessPointsDataTest, properDataIsSet)
{
    auto apData = AccessPointsDataI::create();

    apData->update(oneItemMap);

    ASSERT_EQ(oneItemMap, apData->getCurrentAPs());
}

TEST(AccessPointsDataTest, changeListIsEmptyWhenNoChanges)
{
    auto apData = AccessPointsDataI::create();

    apData->update(twoItemsMap);
    const ChangeList_t changeList = apData->update(twoItemsMap);

    ASSERT_THAT(changeList, IsEmpty());

    ASSERT_EQ(twoItemsMap, apData->getCurrentAPs());
}

TEST(AccessPointsDataTest, APAdditionIsDetected)
{
    auto apData = AccessPointsDataI::create();

    apData->update(oneItemMap);
    const ChangeList_t changeList = apData->update(twoItemsMap);

    ASSERT_THAT(changeList, SizeIs(1));
    ASSERT_EQ(changeList[0].changeType, APDataChange::NewAP);
    ASSERT_EQ(changeList[0].newAP, AP2);

    ASSERT_EQ(twoItemsMap, apData->getCurrentAPs());
}

TEST(AccessPointsDataTest, TwoAPAdditionIsDetected)
{
    auto apData = AccessPointsDataI::create();

    const ChangeList_t changeList = apData->update(twoItemsMap);

    ASSERT_THAT(changeList, SizeIs(2));
    ASSERT_EQ(changeList[0].changeType, APDataChange::NewAP);
    ASSERT_EQ(changeList[1].changeType, APDataChange::NewAP);
}

TEST(AccessPointsDataTest, APRemovalIsDetected)
{
    auto apData = AccessPointsDataI::create();

    apData->update(twoItemsMap);
    const ChangeList_t changeList = apData->update(oneItemMap);

    ASSERT_THAT(changeList, SizeIs(1));
    ASSERT_EQ(changeList[0].changeType, APDataChange::RemovedAP);
    ASSERT_EQ(changeList[0].oldAP, AP2);

    ASSERT_EQ(oneItemMap, apData->getCurrentAPs());
}

TEST(AccessPointsDataTest, APModificationIsDetected)
{
    auto apData = AccessPointsDataI::create();

    apData->update(twoItemsMap);

    auto moddifiedAP2Map = twoItemsMap;

    // modify channel and SNR in AP2
    moddifiedAP2Map[AP2.SSID].channel++;
    moddifiedAP2Map[AP2.SSID].SNR++;

    const ChangeList_t changeList = apData->update(moddifiedAP2Map);

    ASSERT_THAT(changeList, SizeIs(1));
    ASSERT_EQ(changeList[0].changeType, APDataChange::APParamsChanged);
    ASSERT_EQ(changeList[0].oldAP, AP2);
    ASSERT_EQ(changeList[0].newAP, moddifiedAP2Map[AP2.SSID]);

    ASSERT_THAT(changeList[0].changedParams, Contains(APDataChange::channnel));
    ASSERT_THAT(changeList[0].changedParams, Contains(APDataChange::SNR));
}
