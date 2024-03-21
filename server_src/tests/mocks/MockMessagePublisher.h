#pragma once

#include <MessagePublisherI.h>

#include <gmock/gmock.h>

class MockMessagePublisher : public MessagePublisherI
{
public:
    MOCK_METHOD(void, sendToSubscribers, (const msg::MsgDescriptor&), (override));
};
