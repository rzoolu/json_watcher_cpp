#include "FileMonitor.h"

#include <Log.h>

#include <cstddef>
#include <filesystem>
#include <stdexcept>
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
    constexpr auto numOfEvents = 128U;

    // Actual max. number of events will differ due to flex-array
    // in inotify_event structure.
    std::array<inotify_event, numOfEvents> eventBuf;

    constexpr auto eventBufSize =
        eventBuf.size() * sizeof(decltype(eventBuf)::value_type);

    bool continueMonitoring = true;
    while (continueMonitoring)
    {
        const auto length = read(m_inotifyFileDesc, eventBuf.data(), eventBufSize);
        if (length < 0)
        {
            LOG(ERROR, "Cannot read inotify descriptor.");

            throw std::runtime_error("inotify file desc. read failure");
        }

        const inotify_event* event = nullptr;
        const std::byte* eventBufBytes =
            reinterpret_cast<std::byte*>(eventBuf.data());

        for (const std::byte* ptr = eventBufBytes; ptr < eventBufBytes + length;
             ptr += sizeof(inotify_event) + event->len)
        {
            event = reinterpret_cast<const inotify_event*>(ptr);

            continueMonitoring = handleFileEvent(event);
            if (!continueMonitoring)
            {
                break;
            }
        }
    }
}

bool FileMonitor::handleFileEvent(const inotify_event* fileEvent)
{

    bool continueMonitoring = true;

    if (fileEvent->mask & IN_MODIFY)
    {
        LOG(INFO, "The file was modified.");
        m_observer.handleFileEvent(FileObserverI::FileModified);
    }
    else if (fileEvent->mask & IN_DELETE_SELF || fileEvent->mask & IN_MOVE_SELF)
    {
        LOG(INFO, "The file was deleted. Stop monitoring file.");
        m_observer.handleFileEvent(FileObserverI::FileDeleted);

        continueMonitoring = false;
    }
    else
    {
        LOG(DEBUG, "Other event on monitored file: {}", fileEvent->mask);
    }

    return continueMonitoring;
}
