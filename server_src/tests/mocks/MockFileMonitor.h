#pragma once

#include <FileMonitorI.h>

#include <gmock/gmock.h>

class MockFileMonitor : public FileMonitorI
{
public:
    MOCK_METHOD(void, startMonitoring, (std::chrono::milliseconds), (override));
};
