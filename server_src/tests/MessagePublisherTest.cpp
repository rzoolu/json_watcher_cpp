#include <MessagePublisherI.h>

#include <gtest/gtest.h>
#include <zmq.hpp>

#include <future>
#include <iostream>

namespace
{
constexpr std::uint16_t UT_TCP_PORT = 8181;

std::string subscribeAndRecieve()
{
    zmq::context_t context;
    zmq::socket_t subscriber(context, zmq::socket_type::sub);

    const std::string tcpTransport("tcp://127.0.0.1:");

    subscriber.connect(tcpTransport + std::to_string(UT_TCP_PORT));
    subscriber.set(zmq::sockopt::subscribe, "");
    // prevent infinite waiting, if test thread was faster
    subscriber.set(zmq::sockopt::rcvtimeo, 1000);

    zmq::message_t receivedMsg;
    const auto recvRes = subscriber.recv(receivedMsg);

    assert(recvRes.has_value() && "No message to receive, perhaps waiting timer in test should be increased.");

    return receivedMsg.to_string();
}

} // namespace

TEST(MessagePublisher, createAndDelete)
{
    auto publisher = MessagePublisherI::create(UT_TCP_PORT);

    ASSERT_TRUE(publisher);
}

TEST(MessagePublisher, sendMessage)
{
    auto publisher = MessagePublisherI::create(UT_TCP_PORT);

    auto subThreadRes = std::async(std::launch::async, subscribeAndRecieve);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    const std::string sentMsg("MESSAGE_BODY");

    publisher->sendToSubscribers(sentMsg);

    const auto receivedMsg = subThreadRes.get();

    ASSERT_EQ(receivedMsg, sentMsg);
}
