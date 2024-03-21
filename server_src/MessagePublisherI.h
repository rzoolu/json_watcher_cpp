#pragma once

#include <Messaging.h>

#include <cstdint>
#include <functional>
#include <memory>

class MessagePublisherI
{
public:
    using MessagePublisherFactory_t =
        std::function<std::unique_ptr<MessagePublisherI>(std::uint16_t)>;

    static MessagePublisherFactory_t create;

    virtual void sendToSubscribers(const msg::MsgDescriptor& msg) = 0;

    virtual ~MessagePublisherI() = default;
};
