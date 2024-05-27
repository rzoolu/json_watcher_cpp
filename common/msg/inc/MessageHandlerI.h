#pragma once

namespace msg
{
struct MsgDescriptor;

// MessageHandlerI used as and observer interface with MessageSubscriberI.
// Allows to handle incoming messages when they are received.

class MessageHandlerI
{
public:
    enum [[nodiscard]] AfterHandleAction
    {
        StopReceiving,
        ContinueReceiving,
    };

public:
    virtual AfterHandleAction handleMessage(const msg::MsgDescriptor& msg) = 0;

protected:
    ~MessageHandlerI() = default;
};
} // namespace msg