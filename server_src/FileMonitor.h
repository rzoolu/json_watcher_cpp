#pragma once

#include <FileMonitorI.h>

#include <filesystem>

class FileMonitor : public FileMonitorI
{
public:
    FileMonitor(const std::filesystem::path& file, FileObserverI& observer);
    ~FileMonitor() override = default;
    void startMonitoring() override;

private:
    std::filesystem::path m_path;
    FileObserverI& m_observer;
};
