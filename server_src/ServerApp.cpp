#include <ServerApp.h>

#include <JsonParserI.h>
#include <Log.h>
#include <MessagePublisherI.h>

#include <algorithm>
#include <cassert>
#include <string>

constexpr auto APP_TCP_PORT = 8282;

ServerApp::ServerApp(const std::filesystem::path& apFile)
    : m_apFile(apFile),
      m_fileMonitor(FileMonitorI::create(apFile, *this)),
      m_jsonParser(JsonParserI::create()),
      m_apData(AccessPointsDataI::create()),
      m_msgPublisher(MessagePublisherI::create(APP_TCP_PORT))
{
}

ServerApp::~ServerApp() = default;

void ServerApp::run()
{
    m_fileMonitor->startMonitoring();
}

void ServerApp::handleFileEvent(FileObserverI::Event event)
{
    switch (event)
    {
    case FileObserverI::FileModified:
        apFileModified();
        break;
    case FileObserverI::FileDeleted:
        LOG(DEBUG, "File {} deleted. Server will stop running.", m_apFile.string());
        break;

    default:
        LOG(ERROR, "Unknown FileObserverI event {}.", static_cast<int>(event));
        assert(false);
        break;
    }
}

void ServerApp::apFileModified()
{
    if (const auto apMap = m_jsonParser->parseFromFile(m_apFile); apMap)
    {
        if (const auto changeList = m_apData->update(*apMap); !changeList.empty())
        {
            sendChangeMessages(changeList);
        }
    }
}

void ServerApp::sendChangeMessages(const ChangeList_t& changeList)
{
    std::vector<std::string> messages;

    const auto changeToMsg = [](const APDataChange& change)
    {
        switch (change.changeType)
        {
        case APDataChange::NewAP:
            return std::string("New AP: ") + change.newAP.SSID;
        case APDataChange::RemovedAP:
            return std::string("Removed AP: ") + change.oldAP.SSID;
        case APDataChange::APParamsChanged:
            return std::string("Changed AP: ") + change.oldAP.SSID;
        default:
            LOG(ERROR, "Unknown APDataChange::Type {}.", static_cast<int>(change.changeType));
            assert(false);
            return std::string();
        }
    };

    std::transform(changeList.begin(), changeList.end(),
                   std::back_inserter(messages),
                   changeToMsg);

    for (const auto& msg : messages)
    {
        m_msgPublisher->sendToSubscribers(msg);
    }
}
