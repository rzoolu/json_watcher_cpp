#pragma once

#include <MessageHandlerI.h>

#include <gmock/gmock.h>

class MockMessageHandler : public msg::MessageHandlerI
{
public:
    MOCK_METHOD(AfterHandleAction, handleMessage, (const msg::MsgDescriptor&), (override));
};
