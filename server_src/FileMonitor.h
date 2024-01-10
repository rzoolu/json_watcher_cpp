#pragma once

#include <FileMonitorI.h>

class FileMonitor : public FileMonitorI
{
public:
    FileMonitor(const std::filesystem::path& file, FileObserverI& observer);
    void startMonitoring() override;

private:
    FileObserverI& m_observer;
    std::filesystem::path m_path;
};
