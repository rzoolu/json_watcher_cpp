
#include <ServerApp.h>

#include "mocks/MockAccessPointsData.h"
#include "mocks/MockFileMonitor.h"
#include "mocks/MockJsonParser.h"
#include "mocks/MockMessagePublisher.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::_;
using testing::Return;
using testing::StartsWith;

namespace
{
constexpr auto DUMMY_JSON_PATH = "/dummy/path/ap.json";
}

class ServerAppTest : public testing::Test
{
protected:
    ServerAppTest()
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
        AccessPointsDataI::create = [this]()
        {
            auto uptr = std::make_unique<MockAccessPointsData>();
            m_mockAccessPointData = uptr.get();
            return uptr;
        };
    }

    void setupMockFileMonitorFactory()
    {
        FileMonitorI::create = [this]([[maybe_unused]] const std::filesystem::path& file,
                                      [[maybe_unused]] FileObserverI& observer)
        {
            auto uptr = std::make_unique<MockFileMonitor>();
            m_mockFileMonitor = uptr.get();
            return uptr;
        };
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
        MessagePublisherI::create = [this]([[maybe_unused]] std::uint16_t tcpPort)
        {
            auto uptr = std::make_unique<MockMessagePublisher>();
            m_mockMessagePublisher = uptr.get();
            return uptr;
        };
    }

protected:
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