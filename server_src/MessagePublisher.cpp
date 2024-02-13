
#include <MessagePublisher.h>

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

void MessagePublisher::sendToSubscribers(const std::string& msg)
{
    if (!m_zmqSocket)
    {
        LOG(ERROR, "ZMQ socket is not bound.");
        return;
    }

    const auto sendRes = m_zmqSocket.send(zmq::buffer(msg), zmq::send_flags::none);

    if (!sendRes)
    {
        LOG(ERROR, "sendToSubscribers failed");
    }
    else
    {
        LOG(DEBUG, "sendToSubscribers msg sent.");
    }
}
