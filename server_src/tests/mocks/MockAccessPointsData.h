#pragma once

#include <AccessPointsDataI.h>

#include <gmock/gmock.h>

class MockAccessPointsData : public AccessPointsDataI
{
public:
    MOCK_METHOD(const AccessPointMap_t&, getCurrentAPs, (), (const, override));
    MOCK_METHOD(ChangeList_t, update, (const AccessPointMap_t&), (override));
};
