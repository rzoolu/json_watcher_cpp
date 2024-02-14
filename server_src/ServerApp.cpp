#include <ServerApp.h>

#include <AccessPointsDataI.h>
#include <JsonParserI.h>
#include <Log.h>
#include <MessagePublisherI.h>

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

    default:
        break;
    }
}

void ServerApp::apFileModified()
{
    if (const auto apMap = m_jsonParser->parseFromFile(m_apFile); apMap)
    {
        if (const auto changeList = m_apData->update(*apMap); !changeList.empty())
        {
            // todo: convert from chagnes to messages
            m_msgPublisher->sendToSubscribers("changeX");
        }
    }
}
