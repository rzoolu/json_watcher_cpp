#include "Messaging.h"

#include <arpa/inet.h>
#include <cstring>

namespace msg
{

zmq::send_result_t sendMsg(zmq::socket_t& socket, const MsgDescriptor& msg)
{
    constexpr auto numOfHeaderFields = 2;

    static_assert(sizeof(msg.header.ifaceId) == sizeof(std::uint32_t));

    const std::uint32_t headerBuf[numOfHeaderFields] = {
        htonl(static_cast<std::uint32_t>(msg.header.ifaceId)),
        htonl(msg.header.msgId)};

    auto sendRes = socket.send(zmq::buffer(&headerBuf, sizeof(headerBuf)),
                               zmq::send_flags::sndmore);
    if (!sendRes)
    {
        return sendRes;
    }

    size_t bytesSent = *sendRes;

    sendRes = socket.send(zmq::buffer(msg.body),
                          zmq::send_flags::none);
    if (!sendRes)
    {
        return sendRes;
    }

    bytesSent += *sendRes;
    return zmq::send_result_t{bytesSent};
}

zmq::recv_result_t receiveMsg(zmq::socket_t& socket, MsgDescriptor& msg)
{
    static_assert(sizeof(msg.header.ifaceId) == sizeof(std::uint32_t));
    static_assert(sizeof(msg.header.msgId) == sizeof(std::uint32_t));

    constexpr auto numOfHeaderFields = 2;
    constexpr auto expectedHeaderSize = numOfHeaderFields * sizeof(std::uint32_t);

    zmq::message_t msgHeader;
    auto recvRes = socket.recv(msgHeader);
    if (!recvRes || !msgHeader.more() || msgHeader.size() != expectedHeaderSize)
    {
        return {};
    }

    std::uint32_t netOrderHeader[numOfHeaderFields];
    static_assert(expectedHeaderSize == sizeof(netOrderHeader));

    std::memcpy(&netOrderHeader, msgHeader.data(), sizeof(netOrderHeader));

    msg.header.ifaceId =
        static_cast<decltype(msg.header.ifaceId)>(ntohl(netOrderHeader[0]));
    msg.header.msgId = ntohl(netOrderHeader[1]);

    zmq::message_t msgBody;
    recvRes = socket.recv(msgBody);

    if (!recvRes)
    {
        return recvRes;
    }

    msg.body = msgBody.to_string();

    // total bytes received
    return zmq::recv_result_t{expectedHeaderSize + *recvRes};
}

} // namespace msg
