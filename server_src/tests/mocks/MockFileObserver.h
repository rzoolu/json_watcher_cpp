#pragma once

#include <FileMonitorI.h>

#include <gmock/gmock.h>

class MockFileObserver : public FileObserverI
{
public:
    MOCK_METHOD(void, handleFileEvent, (FileObserverI::Event), (override));
};