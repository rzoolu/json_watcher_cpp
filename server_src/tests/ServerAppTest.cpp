
#include <ServerApp.h>

#include <ApWatchI.pb.h>
#include <Message.h>

#include "mocks/MockAccessPointsData.h"
#include "mocks/MockFactory.h"
#include "mocks/MockFileMonitor.h"
#include "mocks/MockJsonParser.h"
#include "mocks/MockMessagePublisher.h"

#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::_;
using testing::Contains;
using testing::MockFunction;
using testing::Return;
using testing::SaveArg;
using testing::StartsWith;

namespace
{
const std::filesystem::path DUMMY_JSON_PATH("/dummy/path/ap.json");
constexpr auto APP_TCP_PORT = 8282;
} // namespace

class ServerAppTest : public testing::Test
{
protected:
    ServerAppTest() : m_mockAccessPointsDataFactory(AccessPointsDataI::create),
                      m_mockFileMonitorFactory(FileMonitorI::create),
                      m_mockJsonParserFactory(JsonParserI::create),
                      m_mockMessagePublisherFactory(msg::MessagePublisherI::create)
    {
        setupMocksInFactories();
    }

    void setupMocksInFactories()
    {
        setupMockAccessPointsDataFactory();
        setupMockFileMonitorFactory();
        setupMockJsonParserFactory();
        setupMockMessagePublisherFactory();
    }

    void setupMockAccessPointsDataFactory()
    {
        auto createMock = [this]()
        {
            auto uptr = std::make_unique<MockAccessPointsData>();
            m_mockAccessPointData = uptr.get();
            return uptr;
        };

        ON_CALL(m_mockAccessPointsDataFactory, Call()).WillByDefault(createMock);
    }

    void setupMockFileMonitorFactory()
    {
        auto createMock = [this]()
        {
            auto uptr = std::make_unique<MockFileMonitor>();
            m_mockFileMonitor = uptr.get();
            return uptr;
        };

        ON_CALL(m_mockFileMonitorFactory, Call(_, _)).WillByDefault(createMock);
    }

    void setupMockJsonParserFactory()
    {
        auto createMock = [this]()
        {
            auto uptr = std::make_unique<MockJsonParser>();
            m_mockJsonParser = uptr.get();
            return uptr;
        };

        ON_CALL(m_mockJsonParserFactory, Call()).WillByDefault(createMock);
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
    MockFactory<AccessPointsDataI::AccessPointsDataFactory_t> m_mockAccessPointsDataFactory;
    MockFactory<FileMonitorI::FileMonitorFactory_t> m_mockFileMonitorFactory;
    MockFactory<JsonParserI::JsonParserFactory_t> m_mockJsonParserFactory;
    MockFactory<msg::MessagePublisherI::MessagePublisherFactory_t> m_mockMessagePublisherFactory;

    MockAccessPointsData* m_mockAccessPointData = nullptr;
    MockFileMonitor* m_mockFileMonitor = nullptr;
    MockJsonParser* m_mockJsonParser = nullptr;
    MockMessagePublisher* m_mockMessagePublisher = nullptr;
};

class ServerAppCreatedTest : public ServerAppTest
{
protected:
    ServerAppCreatedTest() : m_serverApp(DUMMY_JSON_PATH)
    {
    }

    ServerApp m_serverApp;
};

TEST_F(ServerAppTest, createDestroyGracefully)
{
    ServerApp app(DUMMY_JSON_PATH);
}

TEST_F(ServerAppTest, whenServerAppIsCreatedMessagePublisherIsCratedOnProperTcpPort)
{
    EXPECT_CALL(m_mockMessagePublisherFactory, Call(APP_TCP_PORT));

    ServerApp app(DUMMY_JSON_PATH);
}

TEST_F(ServerAppTest, whenServerAppIsCreatedFileMonitorIsCratedWithProperPath)
{
    EXPECT_CALL(m_mockFileMonitorFactory, Call(DUMMY_JSON_PATH, _));

    ServerApp app(DUMMY_JSON_PATH);
}

TEST_F(ServerAppCreatedTest, whenServerAppIsStartedFileMonitoringBegins)
{
    EXPECT_CALL(*m_mockFileMonitor, startMonitoring);

    m_serverApp.run();
}

TEST_F(ServerAppCreatedTest, whenFileModificationIsReportedJsonIsParsed)
{
    EXPECT_CALL(*m_mockJsonParser, parseFromFile(_));

    auto* serverAppAsFileObserver = static_cast<FileObserverI*>(&m_serverApp);

    serverAppAsFileObserver->handleFileEvent(FileObserverI::FileModified);
}

TEST_F(ServerAppCreatedTest, whenNewJsonContentIsInvalidApDataIsNotUpdated)
{
    const std::optional<AccessPointMap_t> invalidParsingResult = std::nullopt;

    EXPECT_CALL(*m_mockJsonParser, parseFromFile).WillOnce(Return(invalidParsingResult));

    EXPECT_CALL(*m_mockAccessPointData, update(_)).Times(0);

    auto* serverAppAsFileObserver = static_cast<FileObserverI*>(&m_serverApp);

    serverAppAsFileObserver->handleFileEvent(FileObserverI::FileModified);
}

TEST_F(ServerAppCreatedTest, whenNewJsonContentIsValidApDataIsUpdatedWithIt)
{
    const AccessPoint validAp{"ssidx", 1, 1};
    const auto validParsingResult =
        std::make_optional<AccessPointMap_t>({{validAp.SSID, validAp}});

    EXPECT_CALL(*m_mockJsonParser, parseFromFile).WillOnce(Return(validParsingResult));

    EXPECT_CALL(*m_mockAccessPointData, update(*validParsingResult));

    auto* serverAppAsFileObserver = static_cast<FileObserverI*>(&m_serverApp);

    serverAppAsFileObserver->handleFileEvent(FileObserverI::FileModified);
}

TEST_F(ServerAppCreatedTest, whenAfterDataUpdateNewApChangeIsDetectedCorrectMsgIsSent)
{
    const AccessPoint validAp{"ssidx", 1, 1};
    const auto validParsingResult =
        std::make_optional<AccessPointMap_t>({{validAp.SSID, validAp}});

    EXPECT_CALL(*m_mockJsonParser, parseFromFile).WillOnce(Return(validParsingResult));

    NewApChange newAp{validAp};
    const APDataChange_t newApChange{newAp};
    const ChangeList_t changeList{newApChange};

    EXPECT_CALL(*m_mockAccessPointData, update(*validParsingResult)).WillOnce(Return(changeList));

    msg::MsgDescriptor msgDesc;

    EXPECT_CALL(*m_mockMessagePublisher, sendToSubscribers(_)).WillOnce(SaveArg<0>(&msgDesc));

    auto* serverAppAsFileObserver = static_cast<FileObserverI*>(&m_serverApp);

    serverAppAsFileObserver->handleFileEvent(FileObserverI::FileModified);

    const auto expectedMsgHeader =
        msg::createMessageHeader(msg::IfaceId::ApWatchI, ApWatchI::NewAp);

    ASSERT_EQ(msgDesc.header, expectedMsgHeader);

    ApWatchI::NewApMsg newApMsg;
    EXPECT_TRUE(newApMsg.ParseFromString(msgDesc.body));

    const auto& msgNewApDetails = newApMsg.ap();
    EXPECT_EQ(msgNewApDetails.ssid(), newAp.newAP.SSID);
    EXPECT_EQ(msgNewApDetails.snr(), newAp.newAP.SNR);
    EXPECT_EQ(msgNewApDetails.channel(), newAp.newAP.channel);
}

TEST_F(ServerAppCreatedTest, whenAfterDataUpdateRemovedApChangeIsDetectedCorrectMsgIsSent)
{
    const AccessPoint validAp{"ssidx", 1, 1};
    const auto validParsingResult =
        std::make_optional<AccessPointMap_t>({{validAp.SSID, validAp}});

    EXPECT_CALL(*m_mockJsonParser, parseFromFile).WillOnce(Return(validParsingResult));

    const AccessPoint removedAp{"removed", 2, 2};
    RemovedApChange removedApChange{removedAp};
    const ChangeList_t changeList{{removedApChange}};

    EXPECT_CALL(*m_mockAccessPointData, update(*validParsingResult)).WillOnce(Return(changeList));

    msg::MsgDescriptor msgDesc;

    EXPECT_CALL(*m_mockMessagePublisher, sendToSubscribers(_)).WillOnce(SaveArg<0>(&msgDesc));

    auto* serverAppAsFileObserver = static_cast<FileObserverI*>(&m_serverApp);

    serverAppAsFileObserver->handleFileEvent(FileObserverI::FileModified);

    const auto expectedMsgHeader =
        msg::createMessageHeader(msg::IfaceId::ApWatchI, ApWatchI::RemovedAp);

    ASSERT_EQ(msgDesc.header, expectedMsgHeader);

    ApWatchI::RemovedApMsg removedApMsg;
    EXPECT_TRUE(removedApMsg.ParseFromString(msgDesc.body));

    const auto& msgRemovedApDetails = removedApMsg.ap();
    EXPECT_EQ(msgRemovedApDetails.ssid(), removedApChange.oldAP.SSID);
    EXPECT_EQ(msgRemovedApDetails.snr(), removedApChange.oldAP.SNR);
    EXPECT_EQ(msgRemovedApDetails.channel(), removedApChange.oldAP.channel);
}

TEST_F(ServerAppCreatedTest, whenAfterDataUpdateApParamsChangeIsDetectedCorrectMsgIsSent)
{
    const AccessPoint validAp{"ssidx", 1, 2};
    const auto validParsingResult =
        std::make_optional<AccessPointMap_t>({{validAp.SSID, validAp}});

    EXPECT_CALL(*m_mockJsonParser, parseFromFile).WillOnce(Return(validParsingResult));

    AccessPoint modifiedAP{validAp};
    modifiedAP.SNR++;
    modifiedAP.channel++;

    ModifiedApParamsChange modifiedApChange{
        validAp,
        modifiedAP,
        {ModifiedApParamsChange::SNR, ModifiedApParamsChange::channnel}};

    const ChangeList_t changeList{{modifiedApChange}};

    EXPECT_CALL(*m_mockAccessPointData, update(*validParsingResult)).WillOnce(Return(changeList));

    msg::MsgDescriptor msgDesc;

    EXPECT_CALL(*m_mockMessagePublisher, sendToSubscribers(_)).WillOnce(SaveArg<0>(&msgDesc));

    auto* serverAppAsFileObserver = static_cast<FileObserverI*>(&m_serverApp);

    serverAppAsFileObserver->handleFileEvent(FileObserverI::FileModified);

    const auto expectedMsgHeader =
        msg::createMessageHeader(msg::IfaceId::ApWatchI, ApWatchI::ModifiedApParams);

    ASSERT_EQ(msgDesc.header, expectedMsgHeader);

    ApWatchI::ModifiedApParamsMsg modifiedApMsg;
    EXPECT_TRUE(modifiedApMsg.ParseFromString(msgDesc.body));

    const auto& msgOldApValues = modifiedApMsg.oldap();
    const auto& msgNewApValues = modifiedApMsg.newap();

    EXPECT_EQ(msgOldApValues.ssid(), modifiedApChange.oldAP.SSID);
    EXPECT_EQ(msgOldApValues.snr(), modifiedApChange.oldAP.SNR);
    EXPECT_EQ(msgOldApValues.channel(), modifiedApChange.oldAP.channel);

    EXPECT_EQ(msgNewApValues.ssid(), modifiedApChange.newAP.SSID);
    EXPECT_EQ(msgNewApValues.snr(), modifiedApChange.newAP.SNR);
    EXPECT_EQ(msgNewApValues.channel(), modifiedApChange.newAP.channel);

    EXPECT_EQ(modifiedApMsg.changedparams_size(), (int)modifiedApChange.changedParams.size());

    EXPECT_THAT(modifiedApMsg.changedparams(), Contains(ApWatchI::ModifiedApParamsMsg::SNR));
    EXPECT_THAT(modifiedApMsg.changedparams(), Contains(ApWatchI::ModifiedApParamsMsg::channnel));
}
