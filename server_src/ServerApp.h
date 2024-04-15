#pragma once

#include "AccessPointsDataI.h"
#include "FileMonitorI.h"

#include <filesystem>
#include <memory>

namespace msg
{
class MessagePublisherI;
}

class FileMonitorI;
class JsonParserI;

class ServerApp : public FileObserverI
{
public:
    ServerApp(const std::filesystem::path& apFile);
    ~ServerApp();

    void run();

private:
    void handleFileEvent(FileObserverI::Event event) override;

private:
    void apFileModified();
    void sendChangeMessages(const ChangeList_t& changeList);

private:
    std::filesystem::path m_apFile;
    std::unique_ptr<FileMonitorI> m_fileMonitor;
    std::unique_ptr<JsonParserI> m_jsonParser;
    std::unique_ptr<AccessPointsDataI> m_apData;
    std::unique_ptr<msg::MessagePublisherI> m_msgPublisher;
};