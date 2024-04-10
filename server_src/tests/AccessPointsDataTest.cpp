#include <AccessPointsDataI.h>

#include <algorithm>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <variant>

using namespace ::testing;

namespace
{

const AccessPoint AP1{"my_ap1", 1, 1};
const AccessPoint AP2{"my_ap2", 2, 2};
const AccessPoint AP3{"my_ap3", 3, 3};

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
    ASSERT_TRUE(std::holds_alternative<NewApChange>(changeList[0]));
    ASSERT_EQ(std::get<NewApChange>(changeList[0]).newAP, AP2);

    ASSERT_EQ(twoItemsMap, apData->getCurrentAPs());
}

TEST(AccessPointsDataTest, TwoAPAdditionIsDetected)
{
    auto apData = AccessPointsDataI::create();

    const ChangeList_t changeList = apData->update(twoItemsMap);

    ASSERT_THAT(changeList, SizeIs(2));
    ASSERT_TRUE(std::holds_alternative<NewApChange>(changeList[0]));
    ASSERT_TRUE(std::holds_alternative<NewApChange>(changeList[1]));
}

TEST(AccessPointsDataTest, APRemovalIsDetected)
{
    auto apData = AccessPointsDataI::create();

    apData->update(twoItemsMap);
    const ChangeList_t changeList = apData->update(oneItemMap);

    ASSERT_THAT(changeList, SizeIs(1));
    ASSERT_TRUE(std::holds_alternative<RemovedApChange>(changeList[0]));
    ASSERT_EQ(std::get<RemovedApChange>(changeList[0]).oldAP, AP2);

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
    ASSERT_TRUE(std::holds_alternative<ModifiedApParamsChange>(changeList[0]));

    const auto& modifiedApChange = std::get<ModifiedApParamsChange>(changeList[0]);

    ASSERT_EQ(modifiedApChange.oldAP, AP2);
    ASSERT_EQ(modifiedApChange.newAP, moddifiedAP2Map[AP2.SSID]);

    ASSERT_THAT(modifiedApChange.changedParams, Contains(ModifiedApParamsChange::channnel));
    ASSERT_THAT(modifiedApChange.changedParams, Contains(ModifiedApParamsChange::SNR));
}

TEST(AccessPointsDataTest, APModificationAndAdditionIsDetected)
{
    auto apData = AccessPointsDataI::create();

    apData->update(twoItemsMap);

    auto moddifiedMap = twoItemsMap;

    moddifiedMap[AP1.SSID].channel++;
    moddifiedMap.emplace(AP3.SSID, AP3);

    const ChangeList_t changeList = apData->update(moddifiedMap);

    ASSERT_THAT(changeList, SizeIs(2));

    const auto paramsModifiedChange =
        std::find_if(changeList.begin(),
                     changeList.end(),
                     [](const APDataChange_t& change)
                     { return std::holds_alternative<ModifiedApParamsChange>(change); });

    ASSERT_TRUE(paramsModifiedChange != changeList.end());
    ASSERT_THAT(std::get<ModifiedApParamsChange>(*paramsModifiedChange).changedParams,
                Contains(ModifiedApParamsChange::channnel));

    const auto newAPChange =
        std::find_if(changeList.begin(),
                     changeList.end(),
                     [](const APDataChange_t& change)
                     { return std::holds_alternative<NewApChange>(change); });

    ASSERT_TRUE(newAPChange != changeList.end());
    ASSERT_EQ(std::get<NewApChange>(*newAPChange).newAP, AP3);
}
