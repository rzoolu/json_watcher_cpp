#pragma once

#include <MessageSubscriberI.h>

#include <cstdint>
#include <zmq.hpp>

namespace msg
{

class MessageSubscriber : public MessageSubscriberI
{
public:
    MessageSubscriber(Host host, TcpPort port, msg::IfaceId ifaceId, MessageHandlerI& msgHandler);
    ~MessageSubscriber() override = default;

    MessageSubscriber(const MessageSubscriber&) = delete;
    MessageSubscriber& operator=(const MessageSubscriber&) = delete;

    void startReceiving() override;

private:
    zmq::context_t m_zmqContext;
    zmq::socket_t m_zmqSocket;

    MessageHandlerI& m_msgHandler;
};
} // namespace msg