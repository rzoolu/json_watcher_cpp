#pragma once

#include <Message.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>

namespace msg
{
class MessageHandlerI;

// Allows to create "subscribe socket", and receive particular messages of interest.
// Usage:
// auto msgSub = msg::MessageSubscriberI::create("host.com",
//                                              APP_TCP_PORT,
//                                              msg::IfaceId::ApWatchI,
//                                              apWatchMsgHandler);
// msgSub->startReceiving();

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
} // namespace msg