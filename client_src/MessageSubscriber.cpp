
#include "MessageSubscriber.h"

#include <ApWatchI.pb.h>
#include <Log.h>
#include <Messaging.h>

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
        auto receivedMsg = msg::receiveMsg(m_zmqSocket);

        if (!receivedMsg)
        {
            LOG(ERROR, "Socket recive error.");
        }

        LOG(DEBUG, "ZMQ socket got message, iface={}, msgId={}",
            msg::toStr(receivedMsg.header.ifaceId), receivedMsg.header.msgId);

        if (receivedMsg.header.ifaceId == msg::IfaceId::ApWatchI)
        {
            switch (receivedMsg.header.msgId)
            {
            case ApWatchI::NewAp:
            {
                ApWatchI::NewApMsg newApMsg;
                newApMsg.ParseFromString(receivedMsg.body);

                LOG(DEBUG, "NewAp message, ssid is: {}", newApMsg.ap().ssid());
            }
            break;

            case ApWatchI::RemovedAp:
            {
                ApWatchI::RemovedApMsg removedApMsg;
                removedApMsg.ParseFromString(receivedMsg.body);

                LOG(DEBUG, "RemovedAp message, ssid is: {}", removedApMsg.ap().ssid());
            }
            break;

            case ApWatchI::ModifiedApParams:
            {
                ApWatchI::ModifiedApParamsMsg modifiedApMsg;
                modifiedApMsg.ParseFromString(receivedMsg.body);

                LOG(DEBUG, "ModifiedApParams message, ssid is: {}", modifiedApMsg.oldap().ssid());
            }
            break;

            default:
                LOG(DEBUG, "Unintresting message: iface={}, msgId={}",
                    msg::toStr(receivedMsg.header.ifaceId), receivedMsg.header.msgId);
                break;
            }
        }
    }
}
