#include "ServerApp.h"

#include <FileMonitor.h>
#include <JsonParserI.h>
#include <Log.h>
#include <MessagePublisherI.h>

ServerApp::ServerApp([[maybe_unused]] std::string_view apFilePath) : m_fileMonitor(),
                                                                     m_jsonParser(),
                                                                     m_msgPublisher()
{
}

ServerApp::~ServerApp() = default;

int ServerApp::run()
{
    return 0;
}