
#include <ServerApp.h>

#include <ApWatchI.pb.h>
#include <Message.h>

#include "mocks/MockFactory.h"
#include "mocks/MockMessagePublisher.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <thread>

using namespace std::filesystem;
using testing::_;
using testing::SaveArg;

namespace
{

using std::chrono::milliseconds;

template <typename... Ops>
void asyncDelayedOperations(const std::filesystem::path& file,
                            milliseconds delay,
                            Ops&&... operations)
{
    std::this_thread::sleep_for(delay);

    // Run every operation, followed be thread sleep
    ((operations(file), std::this_thread::sleep_for(delay)), ...);
}

template <typename... Ops>
auto scheduleDeleyedFileOperations(const std::filesystem::path& file,
                                   milliseconds delay,
                                   Ops... operations)
{
    std::thread t(asyncDelayedOperations<Ops...>,
                  std::cref(file),
                  std::move(delay),
                  std::move(operations)...);

    return t;
}

const path TEST_MONITORED_JSON_PATH = "./e2e_test.json";

const path TEST_3APS_JSON_PATH = "./e2e_test_3APs.json";
const path TEST_2APS_JSON_PATH = "./e2e_test_2APs.json";
const path TEST_1AP_JSON_PATH = "./e2e_test_1AP.json";
const path TEST_1AP_mod_ch_JSON_PATH = "./e2e_test_1APmodified_ch.json";

void deleteFile(const path& file)
{
    std::cerr << "e23 deleteFile \n";

    remove(file);
}

void overwriteFile(const path& src, const path& dst)
{
    std::cerr << "e2e overwriteFile \n";
    if (!exists(src))
    {
        throw std::logic_error("E2E overwriteFile: src file doesn't exists.");
    }
    copy_file(src, dst, copy_options::overwrite_existing);
}
} // namespace

class ServerAppE2ETest : public testing::Test
{
protected:
    ServerAppE2ETest() : m_mockMessagePublisherFactory(msg::MessagePublisherI::create)
    {
        setupMockMessagePublisherFactory();
    }

    void setupMockMessagePublisherFactory()
    {
        auto createMock = [this]()
        {
            auto uptr = std::make_unique<MockMessagePublisher>();
            m_mockMessagePublisher = uptr.get();
            return uptr;
        };

        ON_CALL(m_mockMessagePublisherFactory, Call(_)).WillByDefault(createMock);
    }

protected:
    MockFactory<msg::MessagePublisherI::MessagePublisherFactory_t> m_mockMessagePublisherFactory;

    MockMessagePublisher* m_mockMessagePublisher = nullptr;
};

class ServerAppCreatedTest : public ServerAppE2ETest
{
protected:
    ServerAppCreatedTest() : m_serverApp(TEST_1AP_JSON_PATH)
    {
    }

    ServerApp m_serverApp;
};

TEST_F(ServerAppE2ETest, createDestroyGracefully)
{
    ServerApp app(TEST_MONITORED_JSON_PATH);
}

TEST_F(ServerAppE2ETest, whenFileDeletedStopMontoringAndExitGracefully)
{
    overwriteFile(TEST_3APS_JSON_PATH, TEST_MONITORED_JSON_PATH);

    ServerApp app(TEST_MONITORED_JSON_PATH);

    const auto operationDelay = milliseconds(100);

    auto fileOpsThread = scheduleDeleyedFileOperations(TEST_MONITORED_JSON_PATH,
                                                       operationDelay,
                                                       deleteFile);
    app.run();

    fileOpsThread.join();
}

TEST_F(ServerAppE2ETest, whenFileIsModifiedButDataHasNotChagnedMsgIsNotSent)
{
    overwriteFile(TEST_3APS_JSON_PATH, TEST_MONITORED_JSON_PATH);

    ServerApp app(TEST_MONITORED_JSON_PATH);

    EXPECT_CALL(*m_mockMessagePublisher, sendToSubscribers(_)).Times(0);

    auto whiteSpaceModification = [](const auto& path)
    {
        std::ofstream file(path, std::ios::app);
        file << " ";
    };

    const auto operationDelay = milliseconds(100);
    auto fileOpsThread = scheduleDeleyedFileOperations(TEST_MONITORED_JSON_PATH,
                                                       operationDelay,
                                                       whiteSpaceModification,
                                                       deleteFile);
    app.run();

    fileOpsThread.join();
}

TEST_F(ServerAppE2ETest, whenFileIsModifiedAndOneApWasRemovedCorretMsgIsSent)
{
    overwriteFile(TEST_3APS_JSON_PATH, TEST_MONITORED_JSON_PATH);

    ServerApp app(TEST_MONITORED_JSON_PATH);

    msg::MsgDescriptor msgDesc;

    EXPECT_CALL(*m_mockMessagePublisher, sendToSubscribers(_)).WillOnce(SaveArg<0>(&msgDesc));

    auto oneApRemovedModification = [](const auto& path)
    {
        overwriteFile(TEST_2APS_JSON_PATH, path);
    };

    const auto operationDelay = milliseconds(100);
    auto fileOpsThread = scheduleDeleyedFileOperations(TEST_MONITORED_JSON_PATH,
                                                       operationDelay,
                                                       oneApRemovedModification,
                                                       deleteFile);
    app.run();
    fileOpsThread.join();

    const auto expectedMsgHeader =
        msg::createMessageHeader(msg::IfaceId::ApWatchI, ApWatchI::RemovedAp);

    ASSERT_EQ(msgDesc.header, expectedMsgHeader);
}

TEST_F(ServerAppE2ETest, whenFileIsModifiedAndOneApWasAddedCorretMsgIsSent)
{
    overwriteFile(TEST_2APS_JSON_PATH, TEST_MONITORED_JSON_PATH);

    ServerApp app(TEST_MONITORED_JSON_PATH);

    msg::MsgDescriptor msgDesc;

    EXPECT_CALL(*m_mockMessagePublisher, sendToSubscribers(_)).WillOnce(SaveArg<0>(&msgDesc));

    auto oneApAddedModification = [](const auto& path)
    {
        overwriteFile(TEST_3APS_JSON_PATH, path);
    };

    const auto operationDelay = milliseconds(100);
    auto fileOpsThread = scheduleDeleyedFileOperations(TEST_MONITORED_JSON_PATH,
                                                       operationDelay,
                                                       oneApAddedModification,
                                                       deleteFile);
    app.run();
    fileOpsThread.join();

    const auto expectedMsgHeader =
        msg::createMessageHeader(msg::IfaceId::ApWatchI, ApWatchI::NewAp);

    ASSERT_EQ(msgDesc.header, expectedMsgHeader);
}

TEST_F(ServerAppE2ETest, whenFileIsModifiedAndOneApWasModifiedCorretMsgIsSent)
{
    overwriteFile(TEST_1AP_JSON_PATH, TEST_MONITORED_JSON_PATH);

    ServerApp app(TEST_MONITORED_JSON_PATH);

    msg::MsgDescriptor msgDesc;

    EXPECT_CALL(*m_mockMessagePublisher, sendToSubscribers(_)).WillOnce(SaveArg<0>(&msgDesc));

    auto oneApAddedModification = [](const auto& path)
    {
        overwriteFile(TEST_1AP_mod_ch_JSON_PATH, path);
    };

    const auto operationDelay = milliseconds(100);
    auto fileOpsThread = scheduleDeleyedFileOperations(TEST_MONITORED_JSON_PATH,
                                                       operationDelay,
                                                       oneApAddedModification,
                                                       deleteFile);
    app.run();
    fileOpsThread.join();

    const auto expectedMsgHeader =
        msg::createMessageHeader(msg::IfaceId::ApWatchI, ApWatchI::ModifiedApParams);

    ASSERT_EQ(msgDesc.header, expectedMsgHeader);
}

TEST_F(ServerAppE2ETest, whenFileIsModifiedAndOneApWasModifiedAndThenAnotherApAddedCorretMsgsAreSent)
{
    overwriteFile(TEST_1AP_mod_ch_JSON_PATH, TEST_MONITORED_JSON_PATH);

    ServerApp app(TEST_MONITORED_JSON_PATH);

    std::vector<msg::MsgDescriptor> msgDescriptorsInOrder;

    EXPECT_CALL(*m_mockMessagePublisher, sendToSubscribers(_))
        .Times(2)
        .WillRepeatedly([&msgDescriptorsInOrder](const auto& msgDesc)
                        { msgDescriptorsInOrder.push_back(msgDesc); });

    auto oneApModified = [](const auto& path)
    {
        overwriteFile(TEST_1AP_JSON_PATH, path);
    };

    auto oneApAdded = [](const auto& path)
    {
        overwriteFile(TEST_2APS_JSON_PATH, path);
    };

    const auto operationDelay = milliseconds(100);
    auto fileOpsThread = scheduleDeleyedFileOperations(TEST_MONITORED_JSON_PATH,
                                                       operationDelay,
                                                       oneApModified,
                                                       oneApAdded,
                                                       deleteFile);
    app.run();
    fileOpsThread.join();

    const auto expectedFirstMsgHeader =
        msg::createMessageHeader(msg::IfaceId::ApWatchI, ApWatchI::ModifiedApParams);

    const auto expectedSecondMsgHeader =
        msg::createMessageHeader(msg::IfaceId::ApWatchI, ApWatchI::NewAp);

    ASSERT_EQ(2, msgDescriptorsInOrder.size()) << "Not all required messages were sent.";

    ASSERT_EQ(msgDescriptorsInOrder[0].header, expectedFirstMsgHeader);
    ASSERT_EQ(msgDescriptorsInOrder[1].header, expectedSecondMsgHeader);
}
