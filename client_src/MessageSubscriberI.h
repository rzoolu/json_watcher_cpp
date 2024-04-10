#pragma once

#include <Messaging.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>

class MessageHandlerI;

class MessageSubscriberI
{
public:
    using TcpPort = std::uint16_t;
    using Host = std::string_view;

    using MessageSubscriberFactory_t =
        std::function<std::unique_ptr<MessageSubscriberI>(Host,
                                                          TcpPort,
                                                          msg::IfaceId,
                                                          MessageHandlerI&)>;

    static MessageSubscriberFactory_t create;

    virtual void startReceiving() = 0;

    virtual ~MessageSubscriberI() = default;
};
