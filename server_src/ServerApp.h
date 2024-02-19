#pragma once

#include <AccessPointsDataI.h>
#include <FileMonitorI.h>

#include <filesystem>
#include <memory>

class FileMonitorI;
class JsonParserI;
class MessagePublisherI;

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
    std::unique_ptr<MessagePublisherI> m_msgPublisher;
};