#include <MessagePublisherI.h>

#include <Message.h>
#include <MessageSendReceive.h>

#include <future>
#include <gtest/gtest.h>
#include <iostream>
#include <zmq.hpp>

namespace
{
constexpr msg::MessagePublisherI::TcpPort UT_TCP_PORT = 8181;

msg::MsgDescriptor subscribeAndRecieve()
{
    zmq::context_t context;
    zmq::socket_t subscriber(context, zmq::socket_type::sub);

    const std::string tcpTransport("tcp://127.0.0.1:");

    subscriber.connect(tcpTransport + std::to_string(UT_TCP_PORT));
    subscriber.set(zmq::sockopt::subscribe, "");
    // prevent infinite waiting, if test thread was faster
    subscriber.set(zmq::sockopt::rcvtimeo, 1000);

    auto recvMsg = msg::receiveMsg(subscriber);

    assert(recvMsg && "No message to receive, perhaps waiting timer in test should be increased.");

    return recvMsg;
}
} // namespace

TEST(MessagePublisher, createAndDelete)
{
    auto publisher = msg::MessagePublisherI::create(UT_TCP_PORT);

    ASSERT_TRUE(publisher);
}

TEST(MessagePublisher, sendMessage)
{
    auto publisher = msg::MessagePublisherI::create(UT_TCP_PORT);

    auto subThreadRes =
        std::async(std::launch::async, subscribeAndRecieve);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    enum DummMsgId
    {
        msgXId = 42
    };

    const auto msgHeader =
        msg::createMessageHeader(msg::IfaceId::ApWatchI, msgXId);

    const std::string msgBody("MESSAGE_BODY");
    const msg::MsgDescriptor sentMsg{msgHeader, msgBody};

    publisher->sendToSubscribers(sentMsg);

    const auto receivedMsg = subThreadRes.get();
    ASSERT_EQ(receivedMsg, sentMsg);
}
