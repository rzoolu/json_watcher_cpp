
#include "MessageSubscriber.h"

#include "MessageSendReceive.h"

#include <ApWatchI.pb.h>
#include <Log.h>
#include <Message.h>
#include <MessageHandlerI.h>

#include <arpa/inet.h>

namespace msg
{

MessageSubscriberI::MessageSubscriberFactory_t MessageSubscriberI::create = [](Host host,
                                                                               TcpPort port,
                                                                               msg::IfaceId ifaceId,
                                                                               MessageHandlerI& msgHandler)
{
    return std::make_unique<MessageSubscriber>(host, port, ifaceId, msgHandler);
};

MessageSubscriber::MessageSubscriber(Host host,
                                     TcpPort port,
                                     msg::IfaceId ifaceId,
                                     MessageHandlerI& msgHandler)
    : m_zmqContext(),
      m_zmqSocket(m_zmqContext, zmq::socket_type::sub),
      m_msgHandler(msgHandler)
{
    const std::string tcpTransport =
        std::string("tcp://") + std::string(host) + ":" + std::to_string(port);

    m_zmqSocket.connect(tcpTransport);

    const auto netIfaceId = htonl(static_cast<std::uint32_t>(ifaceId));

    m_zmqSocket.set(zmq::sockopt::subscribe,
                    zmq::const_buffer(&netIfaceId, sizeof(netIfaceId)));

    LOG(DEBUG, "MessageSubscriber connected to {}", tcpTransport);
}

void MessageSubscriber::startReceiving()
{
    if (!m_zmqSocket)
    {
        LOG(ERROR, "ZMQ socket is not connected.");
        return;
    }

    while (true)
    {
        auto receivedMsg = msg::receiveMsg(m_zmqSocket);

        if (!receivedMsg)
        {
            LOG(ERROR, "Socket receive error.");
        }

        LOG(DEBUG, "ZMQ socket got message, iface={}, msgId={}",
            msg::toStr(receivedMsg.header.ifaceId), receivedMsg.header.msgId);

        m_msgHandler.handleMessage(receivedMsg);
    }
}
} // namespace msg