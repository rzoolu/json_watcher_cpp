#include "FileMonitor.h"

#include <Log.h>

#include <cstddef>
#include <exception>
#include <sys/inotify.h>
#include <unistd.h>

FileMonitorI::FileMonitorFactory_t FileMonitorI::create = [](const std::filesystem::path& file,
                                                             FileObserverI& observer)
{
    return std::make_unique<FileMonitor>(file, observer);
};

FileMonitor::FileMonitor(const std::filesystem::path& file, FileObserverI& observer)
    : m_path(file),
      m_observer(observer),
      m_inotifyFileDesc(INVALID_FD),
      m_fileWatchDesc(INVALID_FD)
{
    if (!std::filesystem::exists(file) ||
        !std::filesystem::is_regular_file(file))
    {
        LOG(ERROR, "File {} doesn't exist.", file.string());
        throw std::runtime_error("File: " + file.string() + " doesn't exists.");
    }

    m_inotifyFileDesc = inotify_init();

    if (m_inotifyFileDesc == INVALID_FD)
    {
        LOG(ERROR, "Cannot create inotify instance.");
        throw std::runtime_error("inotify_init failure.");
    }
}

FileMonitor::~FileMonitor()
{
    if (m_inotifyFileDesc != INVALID_FD)
    {
        if (m_fileWatchDesc != INVALID_FD)
        {
            inotify_rm_watch(m_inotifyFileDesc, m_fileWatchDesc);
        }

        close(m_inotifyFileDesc);
    }
}

void FileMonitor::startMonitoring()
{
    m_fileWatchDesc = inotify_add_watch(m_inotifyFileDesc, m_path.c_str(),
                                        IN_MODIFY | IN_DELETE_SELF | IN_MOVE_SELF);

    if (m_fileWatchDesc == INVALID_FD)
    {
        LOG(ERROR, "Cannot add inotify watch for {}", m_path.string());
        throw std::runtime_error("inotify_add_watch failure");
    }

    LOG(DEBUG, "Monitoring started for file {}", m_path.string());

    waitForFileEvents();
}

void FileMonitor::waitForFileEvents()
{
    bool fileExists = true;

    while (fileExists)
    {
        std::byte eventBuf[1024]
            __attribute__((aligned(__alignof__(struct inotify_event))));

        const auto length = read(m_inotifyFileDesc, eventBuf, sizeof(eventBuf));
        if (length < 0)
        {
            LOG(ERROR, "Cannot read inotify descriptor.");

            throw std::runtime_error("inotify file desc. read failure");
        }

        const struct inotify_event* event = NULL;

        for (const std::byte* ptr = eventBuf; ptr < eventBuf + length;
             ptr += sizeof(struct inotify_event) + event->len)
        {
            event = reinterpret_cast<const struct inotify_event*>(ptr);

            if (event->mask & IN_MODIFY)
            {
                LOG(INFO, "The file was modified.");
                m_observer.handleFileEvent(FileObserverI::FileModified);
            }
            else if (event->mask & IN_DELETE_SELF || event->mask & IN_MOVE_SELF)
            {
                LOG(INFO, "The file was deleted. Stop monitoring file.");
                m_observer.handleFileEvent(FileObserverI::FileDeleted);

                fileExists = false;
                break;
            }
            else
            {
                LOG(DEBUG, "Other event on monitored file: {}", event->mask);
            }
        }
    }
}