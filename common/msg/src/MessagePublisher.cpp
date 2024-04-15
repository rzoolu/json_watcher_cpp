
#include "MessagePublisher.h"

#include <Log.h>

#include <string>

MessagePublisherI::MessagePublisherFactory_t MessagePublisherI::create = [](std::uint16_t tcpPort)
{
    return std::make_unique<MessagePublisher>(tcpPort);
};

MessagePublisher::MessagePublisher(std::uint16_t tcpPort) : m_zmqContext(),
                                                            m_zmqSocket(m_zmqContext, zmq::socket_type::pub)
{
    std::string tcpTransport("tcp://*:");
    tcpTransport.append(std::to_string(tcpPort));

    m_zmqSocket.bind(tcpTransport);

    LOG(DEBUG, "MessagePublisher bound to {}", tcpTransport);
}

void MessagePublisher::sendToSubscribers(const msg::MsgDescriptor& msg)
{
    if (!m_zmqSocket)
    {
        LOG(ERROR, "ZMQ socket is not bound.");
        return;
    }

    const auto sendRes = msg::sendMsg(m_zmqSocket, msg);
    if (!sendRes)
    {
        LOG(ERROR, "sendToSubscribers msg[ifaceId = {}, msgId = {}] failed.",
            toStr(msg.header.ifaceId), msg.header.msgId);
    }
    else
    {
        LOG(DEBUG, "sendToSubscriber, {} bytes sent, (ifaceId={}, msgId={}).",
            *sendRes, toStr(msg.header.ifaceId), msg.header.msgId);
    }
}
