#pragma once

#include <zmq.hpp>

namespace msg
{
class MsgDescriptor;

zmq::send_result_t sendMsg(zmq::socket_t& socket, const MsgDescriptor& msg);
MsgDescriptor receiveMsg(zmq::socket_t& socket);

} // namespace msg