#pragma once

#include "MessageSubscriberI.h"

#include <zmq.hpp>

#include <cstdint>

class MessageSubscriber : public MessageSubscriberI
{
public:
    MessageSubscriber(Host host, TcpPort port, msg::IfaceId ifaceId, MessageHandlerI& msgHandler);
    ~MessageSubscriber() override = default;

    void startReceiving() override;

private:
    zmq::context_t m_zmqContext;
    zmq::socket_t m_zmqSocket;

    MessageHandlerI& m_msgHandler;
};
