#pragma once

#include <memory>
#include <string_view>

class JsonParserI;
class FileMonitorI;
class MessagePublisherI;

class ServerApp
{
public:
    ServerApp(std::string_view apFilePath);
    ~ServerApp();

    int run();

private:
    std::unique_ptr<FileMonitorI> m_fileMonitor;
    std::unique_ptr<JsonParserI> m_jsonParser;
    std::unique_ptr<MessagePublisherI> m_msgPublisher;
};