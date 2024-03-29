#pragma once

#include "InterfaceIds.h"

#include <google/protobuf/message_lite.h>
#include <zmq.hpp>

#include <string>

namespace msg
{

constexpr std::uint32_t MSG_ID_UNUSED = 0;

struct MsgHeader
{
    IfaceId ifaceId = IfaceId::UNUSED;
    std::uint32_t msgId = MSG_ID_UNUSED;

    bool operator==(const MsgHeader&) const = default;
};

struct MsgDescriptor
{
    MsgHeader header;
    std::string body;

    bool operator==(const MsgDescriptor&) const = default;
};

template <typename MsgId>
constexpr MsgHeader createMessageHeader(IfaceId iface, MsgId msg)
{
    static_assert(std::is_enum_v<MsgId>,
                  "Provide enum MsgId value, from Iface.proto file.");
    static_assert(sizeof(MsgId) == sizeof(MsgHeader{}.msgId),
                  "enum MsgId not compatible with MsgHeader.msgId");

    if (iface != IfaceId::UNUSED &&
        static_cast<std::uint32_t>(msg) != MSG_ID_UNUSED)
    {
        return MsgHeader{iface, static_cast<std::uint32_t>(msg)};
    }
    else
    {
        return MsgHeader{};
    }
}

constexpr bool isHeaderValid(const MsgHeader& header)
{
    return header.ifaceId != IfaceId::UNUSED &&
           header.msgId != MSG_ID_UNUSED;
}

template <typename MsgId>
MsgDescriptor createMsgDescriptor(IfaceId ifaceId,
                                  MsgId msgId,
                                  google::protobuf::MessageLite& msgBody)
{

    const auto header = createMessageHeader(ifaceId, msgId);

    if (isHeaderValid(header))
    {
        return MsgDescriptor{header, msgBody.SerializeAsString()};
    }
    else
    {
        return MsgDescriptor{};
    }
}

zmq::send_result_t sendMsg(zmq::socket_t& socket, const MsgDescriptor& msg);
zmq::recv_result_t receiveMsg(zmq::socket_t& socket, MsgDescriptor& msg);

} // namespace msg