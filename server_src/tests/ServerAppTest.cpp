
#include <ServerApp.h>

#include "mocks/MockAccessPointsData.h"
#include "mocks/MockFileMonitor.h"
#include "mocks/MockFileMonitorFactory.h"
#include "mocks/MockJsonParser.h"
#include "mocks/MockMessagePublisher.h"
#include "mocks/MockMessagePublisherFactory.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>

using testing::_;
using testing::MockFunction;
using testing::Return;
using testing::StartsWith;

namespace
{
const std::filesystem::path DUMMY_JSON_PATH("/dummy/path/ap.json");
constexpr auto APP_TCP_PORT = 8282;
} // namespace

class ServerAppTest : public testing::Test
{
protected:
    ServerAppTest()
    {
        setupMocksInFactories();
    }

    ~ServerAppTest()
    {
        restoreOrigianalFactories();
    }

    void setupMocksInFactories()
    {
        setupMockAccessPointsDataFactory();
        setupMockFileMonitorFactory();
        setupMockJsonParserFactory();
        setupMockMessagePublisherFactory();
    }

    void restoreOrigianalFactories()
    {
        restoreMessagePublisherFactory();
        restoreFileMonitorFactory();
    }

    void setupMockAccessPointsDataFactory()
    {
        AccessPointsDataI::create = [this]()
        {
            auto uptr = std::make_unique<MockAccessPointsData>();
            m_mockAccessPointData = uptr.get();
            return uptr;
        };
    }

    void setupMockFileMonitorFactory()
    {
        auto createMock = [this]([[maybe_unused]] const std::filesystem::path& file,
                                 [[maybe_unused]] FileObserverI& observer)
        {
            auto uptr = std::make_unique<MockFileMonitor>();
            m_mockFileMonitor = uptr.get();
            return uptr;
        };

        useFileMonitorFactoryMock(m_mockFileMonitorFactory);
        ON_CALL(m_mockFileMonitorFactory, Call(_, _)).WillByDefault(createMock);
    }

    void setupMockJsonParserFactory()
    {
        JsonParserI::create = [this]()
        {
            auto uptr = std::make_unique<MockJsonParser>();
            m_mockJsonParser = uptr.get();
            return uptr;
        };
    }

    void setupMockMessagePublisherFactory()
    {
        auto createMock = [this]()
        {
            auto uptr = std::make_unique<MockMessagePublisher>();
            m_mockMessagePublisher = uptr.get();
            return uptr;
        };

        useMessagePublisherFactoryMock(m_mockMessagePublisherFactory);
        ON_CALL(m_mockMessagePublisherFactory, Call(_)).WillByDefault(createMock);
    }

protected:
    MockAccessPointsData* m_mockAccessPointData = nullptr;
    MockFileMonitor* m_mockFileMonitor = nullptr;
    MockJsonParser* m_mockJsonParser = nullptr;
    MockMessagePublisher* m_mockMessagePublisher = nullptr;

    MessagePublisherFactoryMock_t m_mockMessagePublisherFactory;
    FileMonitorFactoryMock_t m_mockFileMonitorFactory;
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

TEST_F(ServerAppCreatedTest, whenAfterDataUpdateChangeIsDetectedMsgIsSent)
{
    const AccessPoint validAp{"ssidx", 1, 1};
    const auto validParsingResult =
        std::make_optional<AccessPointMap_t>({{validAp.SSID, validAp}});

    EXPECT_CALL(*m_mockJsonParser, parseFromFile).WillOnce(Return(validParsingResult));

    const APDataChange newApChange{APDataChange::NewAP, {}, validAp, {}};
    const ChangeList_t changeList{newApChange};

    EXPECT_CALL(*m_mockAccessPointData, update(*validParsingResult)).WillOnce(Return(changeList));

    EXPECT_CALL(*m_mockMessagePublisher, sendToSubscribers(StartsWith("New AP"))).Times(1);

    auto* serverAppAsFileObserver = static_cast<FileObserverI*>(&m_serverApp);

    serverAppAsFileObserver->handleFileEvent(FileObserverI::FileModified);
}
