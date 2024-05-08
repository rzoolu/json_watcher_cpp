#pragma once

#include "FileMonitorI.h"

#include <cstdint>
#include <filesystem>

struct inotify_event;

class FileMonitor : public FileMonitorI
{
public:
    FileMonitor(const std::filesystem::path& file, FileObserverI& observer);
    ~FileMonitor() override;
    void startMonitoring(std::chrono::milliseconds stabiltyTimeOut) override;

private:
    void initInotify();
    void cleanupInotify();
    void handleFileSystemEvents();

    enum WaitingStatus
    {
        EventsReady,
        NoEvents,
        TimeOut,
    };
    WaitingStatus waitForInotifyEvents(int pollTimeOutInMs);
    void handleInotifyEvents();
    void handleInotifyFileEvent(const inotify_event* fileEvent);
    void handleInotifyParentDirEvent(const inotify_event* parentDirEvent);
    void commitFileEvent();

private:
    enum CurrentFileState
    {
        NoChangesDetected,
        PendingModification,
        PendingDeletion,
        Deleted
    };

    std::filesystem::path m_path;
    FileObserverI& m_observer;
    int m_inotifyFileDesc;
    int m_fileWatchDesc;
    int m_parentDirWatchDesc;
    std::chrono::milliseconds m_stabiltyTimeOut;
    CurrentFileState m_fileState;
};
