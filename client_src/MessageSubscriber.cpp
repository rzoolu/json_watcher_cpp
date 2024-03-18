
#include "MessageSubscriber.h"

#include <ApWatchI.pb.h>
#include <Log.h>

MessageSubscriberI::MessageSubscriberFactory_t MessageSubscriberI::create = [](Host host, TcpPort port)
{
    return std::make_unique<MessageSubscriber>(host, port);
};

MessageSubscriber::MessageSubscriber(Host host, TcpPort port) : m_zmqContext(),
                                                                m_zmqSocket(m_zmqContext, zmq::socket_type::sub)
{
    const std::string tcpTransport =
        std::string("tcp://") + std::string(host) + ":" + std::to_string(port);

    m_zmqSocket.connect(tcpTransport);

    m_zmqSocket.set(zmq::sockopt::subscribe, "");

    LOG(DEBUG, "MessageSubscriber connected to {}", tcpTransport);
}

void MessageSubscriber::startReceiving()
{
    if (!m_zmqSocket)
    {
        LOG(ERROR, "ZMQ socket is not connected.");
        return;
    }

    while (1)
    {
        zmq::message_t msg;
        const auto ret = m_zmqSocket.recv(msg);

        if (!ret)
        {
            LOG(ERROR, "ZMQ socket recive error.");
        }

        LOG(DEBUG, "ZMQ socket got message, size is: {}", *ret);

        ApWatchI::Msg apWatchMsg;
        apWatchMsg.ParseFromString(msg.to_string_view());

        switch (apWatchMsg.msg_case())
        {
        case ApWatchI::Msg::kNewAp:
            LOG(DEBUG, "kNewAp message, ssid is: {}", apWatchMsg.newap().ap().ssid());

            break;
        case ApWatchI::Msg::kRemovedAp:
            LOG(DEBUG, "kRemovedAp message, ssid is: {}", apWatchMsg.removedap().ap().ssid());

            break;
        case ApWatchI::Msg::kModifiedAp:
            LOG(DEBUG, "kModifiedAp message, ssid is: {}", apWatchMsg.modifiedap().oldap().ssid());
            break;
        default:
            break;
        }
    }
}
