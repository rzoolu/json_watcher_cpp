#pragma once

#include "MessageSubscriberI.h"

#include <zmq.hpp>

#include <cstdint>

class MessageSubscriber : public MessageSubscriberI
{
public:
    MessageSubscriber(Host host, TcpPort port);
    ~MessageSubscriber() override = default;

    void startReceiving() override;

private:
    zmq::context_t m_zmqContext;
    zmq::socket_t m_zmqSocket;
};
