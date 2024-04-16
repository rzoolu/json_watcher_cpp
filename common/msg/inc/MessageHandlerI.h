#pragma once

namespace msg
{
class MsgDescriptor;

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