#pragma once

#include <MessagePublisherI.h>

#include <cstdint>
#include <zmq.hpp>

namespace msg
{
class MessagePublisher : public MessagePublisherI
{
public:
    MessagePublisher(TcpPort tcpPort);
    ~MessagePublisher() override = default;

    MessagePublisher(const MessagePublisher&) = delete;
    MessagePublisher& operator=(const MessagePublisher&) = delete;

    void sendToSubscribers(const MsgDescriptor& msg) override;

private:
    zmq::context_t m_zmqContext;
    zmq::socket_t m_zmqSocket;
};
} // namespace msg
