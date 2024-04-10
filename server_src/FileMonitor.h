#pragma once

#include "FileMonitorI.h"

#include <filesystem>

class FileMonitor : public FileMonitorI
{
public:
    FileMonitor(const std::filesystem::path& file, FileObserverI& observer);
    ~FileMonitor() override;
    void startMonitoring() override;

private:
    void waitForFileEvents();

private:
    static constexpr int INVALID_FD = -1;

    std::filesystem::path m_path;
    FileObserverI& m_observer;

    int m_inotifyFileDesc;
    int m_fileWatchDesc;
};
