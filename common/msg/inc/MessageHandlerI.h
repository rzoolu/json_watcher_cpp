#pragma once

namespace msg
{
class MsgDescriptor;

class MessageHandlerI
{
public:
    virtual void handleMessage(const msg::MsgDescriptor& msg) = 0;

protected:
    ~MessageHandlerI() = default;
};
} // namespace msg