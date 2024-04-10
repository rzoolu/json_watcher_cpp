#pragma once

#include "MessagePublisherI.h"

#include <cstdint>
#include <zmq.hpp>

class MessagePublisher : public MessagePublisherI
{
public:
    MessagePublisher(std::uint16_t tcpPort);
    ~MessagePublisher() override = default;

    void sendToSubscribers(const msg::MsgDescriptor& msg) override;

private:
    zmq::context_t m_zmqContext;
    zmq::socket_t m_zmqSocket;
};
