#pragma once

#include <cstdint>
#include <functional>
#include <memory>

namespace msg
{
struct MsgDescriptor;

// Allows to create "publish socket", and publish/sent arbitrary message
// to all active subscribers.

class MessagePublisherI
{
public:
    using TcpPort = std::uint16_t;

    using MessagePublisherFactory_t =
        std::function<std::unique_ptr<MessagePublisherI>(TcpPort)>;

    static MessagePublisherFactory_t create;

    virtual void sendToSubscribers(const msg::MsgDescriptor& msg) = 0;

    virtual ~MessagePublisherI() = default;
};
} // namespace msg