#include <MessageSubscriberI.h>

#include "mocks/MockMessageHandler.h"

#include <Message.h>
#include <MessageSendReceive.h>

#include <future>
#include <gtest/gtest.h>
#include <iostream>
#include <zmq.hpp>

using testing::Return;

namespace
{
constexpr std::uint16_t UT_TCP_PORT = 8181;

} // namespace

TEST(MessageSubscriber, createAndDelete)
{
    MockMessageHandler msgHandlerMock;

    auto subscriber = msg::MessageSubscriberI::create("127.0.0.1",
                                                      UT_TCP_PORT,
                                                      msg::IfaceId::ApWatchI,
                                                      msgHandlerMock);

    ASSERT_TRUE(subscriber);
}

class MessageSubscriberTest : public testing::Test
{
protected:
    MessageSubscriberTest() : m_pubContext(),
                              m_pubSocket(m_pubContext, zmq::socket_type::pub) {}

    void initPublisher()
    {
        std::string tcpTransport("tcp://*:");
        tcpTransport.append(std::to_string(UT_TCP_PORT));

        m_pubSocket.bind(tcpTransport);

        ASSERT_TRUE(m_pubSocket) << "Publisher ZMQ socket is not bound.";
    }

    void publishMessage(const msg::MsgDescriptor& msg)
    {
        const auto sendRes = msg::sendMsg(m_pubSocket, msg);

        ASSERT_TRUE(sendRes) << "Message publishing failure.";
    }

protected:
    zmq::context_t m_pubContext;
    zmq::socket_t m_pubSocket;
};

namespace
{
void subscribeAndRecieve(msg::MsgDescriptor publishedMsg)
{
    MockMessageHandler msgHandlerMock;

    EXPECT_CALL(msgHandlerMock, handleMessage(publishedMsg))
        .WillOnce(Return(msg::MessageHandlerI::StopReceiving));

    auto subscriber = msg::MessageSubscriberI::create("127.0.0.1",
                                                      UT_TCP_PORT,
                                                      msg::IfaceId::ApWatchI,
                                                      msgHandlerMock);

    subscriber->startReceiving();
}
} // namespace

TEST_F(MessageSubscriberTest, receivePublishedMessage)
{
    initPublisher();

    enum DummMsgId
    {
        msgXId = 42
    };
    const msg::MsgDescriptor publishedMsg{
        msg::createMessageHeader(msg::IfaceId::ApWatchI, msgXId),
        "MESSAGE_BODY"};

    auto subThreadRes =
        std::async(std::launch::async, subscribeAndRecieve, publishedMsg);

    // wait for receving thread to be blocked on socket.recv()
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    publishMessage(publishedMsg);

    // wait for exiting MessageSubscriber::startReceiving(),
    // but prevent deadlock if published messge was not received after timeout.
    const auto status = subThreadRes.wait_for(std::chrono::milliseconds(1000));

    if (status != std::future_status::ready)
    {
        EXPECT_TRUE(false) << "Unexpected thread race, force terminate to prevent deadlock.";
        std::terminate();
    }
}
