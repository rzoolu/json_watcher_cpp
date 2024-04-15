#pragma once

#include <MessagePublisherI.h>

#include <cstdint>
#include <zmq.hpp>

namespace msg
{
class MessagePublisher : public MessagePublisherI
{
public:
    MessagePublisher(std::uint16_t tcpPort);
    ~MessagePublisher() override = default;

    void sendToSubscribers(const MsgDescriptor& msg) override;

private:
    zmq::context_t m_zmqContext;
    zmq::socket_t m_zmqSocket;
};
} // namespace msg
