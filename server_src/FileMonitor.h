#pragma once

#include "FileMonitorI.h"

#include <filesystem>

struct inotify_event;

class FileMonitor : public FileMonitorI
{
public:
    FileMonitor(const std::filesystem::path& file, FileObserverI& observer);
    ~FileMonitor() override;
    void startMonitoring() override;

private:
    void waitForFileEvents();
    bool handleFileEvent(const inotify_event* fileEvent);

private:
    static constexpr int INVALID_FD = -1;

    std::filesystem::path m_path;
    FileObserverI& m_observer;

    int m_inotifyFileDesc;
    int m_fileWatchDesc;
};
