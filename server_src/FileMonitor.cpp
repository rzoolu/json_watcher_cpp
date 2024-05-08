#include "FileMonitor.h"

#include <Log.h>

#include <cerrno>
#include <cstddef>
#include <filesystem>
#include <poll.h>
#include <stdexcept>
#include <sys/inotify.h>
#include <unistd.h>

namespace
{
constexpr int WAIT_INDEFINTELY = -1;
constexpr int INVALID_FD = -1;

int stabilityTimeOutToPollTimeOut(std::chrono::milliseconds stabilityTimeOut)
{
    int pollTimeOutInMs = 0;
    if (std::numeric_limits<int>::max() < stabilityTimeOut.count())
    {
        pollTimeOutInMs = std::numeric_limits<int>::max();
    }
    else if (stabilityTimeOut <= FileMonitorI::NO_STABILIZATION)
    {
        pollTimeOutInMs = WAIT_INDEFINTELY;
    }
    else
    {
        pollTimeOutInMs = stabilityTimeOut.count();
    }

    return pollTimeOutInMs;
}
} // namespace

FileMonitorI::FileMonitorFactory_t FileMonitorI::create = [](const std::filesystem::path& file,
                                                             FileObserverI& observer)
{
    return std::make_unique<FileMonitor>(file, observer);
};

FileMonitor::FileMonitor(const std::filesystem::path& file,
                         FileObserverI& observer)
    : m_path(file),
      m_observer(observer),
      m_inotifyFileDesc(INVALID_FD),
      m_fileWatchDesc(INVALID_FD),
      m_parentDirWatchDesc(INVALID_FD),
      m_stabiltyTimeOut(NO_STABILIZATION),
      m_fileState(NoChangesDetected)
{
}

void FileMonitor::initInotify()
{
    m_inotifyFileDesc = inotify_init1(IN_NONBLOCK);

    if (m_inotifyFileDesc == INVALID_FD)
    {
        LOG(ERROR, "Cannot create inotify instance.");
        throw std::runtime_error("inotify_init failure.");
    }

    if (!std::filesystem::exists(m_path) || !std::filesystem::is_regular_file(m_path))
    {
        LOG(ERROR, "File {} doesn't exist or it's not regular file.", m_path.string());
        throw std::runtime_error(m_path.string() + " doesn't exist.");
    }

    m_fileWatchDesc = inotify_add_watch(m_inotifyFileDesc, m_path.c_str(),
                                        IN_MODIFY | IN_DELETE_SELF);

    if (m_fileWatchDesc == INVALID_FD)
    {
        LOG(ERROR, "Cannot add inotify watch for {}", m_path.string());
        throw std::runtime_error("inotify_add_watch failure");
    }

    std::filesystem::path parentDir;

    if (!m_path.has_parent_path())
    {
        parentDir = std::filesystem::current_path();
    }
    else
    {
        parentDir = m_path.parent_path();
    }

    m_parentDirWatchDesc = inotify_add_watch(m_inotifyFileDesc, parentDir.c_str(),
                                             IN_MOVED_TO | IN_MOVED_FROM | IN_CREATE);

    if (m_parentDirWatchDesc == INVALID_FD)
    {
        LOG(ERROR, "Cannot add inotify watch for parent dir: {}", parentDir.string());
        throw std::runtime_error("inotify_add_watch for parent dir failure");
    }
}

FileMonitor::~FileMonitor()
{
    cleanupInotify();
}

void FileMonitor::cleanupInotify()
{
    if (m_inotifyFileDesc != INVALID_FD)
    {
        if (m_fileWatchDesc != INVALID_FD)
        {
            inotify_rm_watch(m_inotifyFileDesc, m_fileWatchDesc);
            m_fileWatchDesc = INVALID_FD;
        }

        if (m_parentDirWatchDesc != INVALID_FD)
        {
            inotify_rm_watch(m_inotifyFileDesc, m_parentDirWatchDesc);
            m_parentDirWatchDesc = INVALID_FD;
        }

        close(m_inotifyFileDesc);
        m_inotifyFileDesc = INVALID_FD;
    }
}

void FileMonitor::startMonitoring(std::chrono::milliseconds stabiltyTimeOut)
{
    initInotify();

    if (stabiltyTimeOut < NO_STABILIZATION)
    {
        stabiltyTimeOut = NO_STABILIZATION;
    }

    m_stabiltyTimeOut = stabiltyTimeOut;

    LOG(INFO, "Monitoring started for file {}, stabiltyTimeOut = {}",
        m_path.string(), stabiltyTimeOut);

    handleFileSystemEvents();

    cleanupInotify();
}

void FileMonitor::handleFileSystemEvents()
{
    const int pollTimeOutInMs = stabilityTimeOutToPollTimeOut(m_stabiltyTimeOut);

    while (m_fileState != Deleted)
    {
        int nextPollTimeOut = pollTimeOutInMs;

        // always wait without stablization timeout for first event
        if (m_fileState == NoChangesDetected)
        {
            nextPollTimeOut = WAIT_INDEFINTELY;
        }

        const auto waitingStatus = waitForInotifyEvents(nextPollTimeOut);

        if (waitingStatus == TimeOut)
        {
            LOG(DEBUG, "Timeout, possibly last event in series, report it.");

            // notify observer about "stabilized" event
            commitFileEvent();
        }
        else if (waitingStatus == EventsReady)
        {
            LOG(DEBUG, "File event(s) ready.");
            // Iterate over all events and call m_observer.handleFileEvent()
            // immediatly for every event if m_stabilityTimeout == NO_STABILIZATION
            handleInotifyEvents();
        }

        // if waitingStatus == NoEvents, just repeat the loop
    }

    LOG(DEBUG, "File deleted, stop monitoring.");
}

FileMonitor::WaitingStatus FileMonitor::waitForInotifyEvents(int pollTimeOutInMs)
{
    pollfd pollInotifyFd{m_inotifyFileDesc, POLLIN, 0};

    LOG(DEBUG, "Waiting for inotify with timeout: {} {}",
        pollTimeOutInMs, pollTimeOutInMs == WAIT_INDEFINTELY ? "(Idefinitely)" : "ms");

    const auto ret = poll(&pollInotifyFd, 1, pollTimeOutInMs);

    // inotify events ready
    if (ret > 0)
    {
        return EventsReady;
    }

    // polling timed out
    if (ret == 0)
    {
        return TimeOut;
    }

    // polling failure
    if (ret == -1 && errno == EINTR)
    {
        return NoEvents;
    }

    LOG(ERROR, "Cannot poll inotify descriptor.");
    throw std::runtime_error("inotify file desc. poll failure");
}

void FileMonitor::handleInotifyEvents()
{
    LOG(DEBUG, "handleInotifyEvents:");

    constexpr auto numOfEvents = 128U;

    // Actual max. number of events will differ due to flex-array
    // in inotify_event structure.
    std::array<inotify_event, numOfEvents> eventBuf;

    constexpr auto eventBufSize =
        eventBuf.size() * sizeof(decltype(eventBuf)::value_type);

    const auto length = read(m_inotifyFileDesc, eventBuf.data(), eventBufSize);
    if (length < 0)
    {
        if (errno == EAGAIN || errno == EINTR)
        {
            // return to main loop and re-try
            return;
        }
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

        LOG(DEBUG, "Ignored event? {}", (event->mask & IN_IGNORED));

        if (event->wd == m_fileWatchDesc)
        {
            handleInotifyFileEvent(event);
        }

        if (event->wd == m_parentDirWatchDesc)
        {
            handleInotifyParentDirEvent(event);
        }

        if (m_stabiltyTimeOut == NO_STABILIZATION)
        {
            commitFileEvent();

            if (m_fileState == Deleted)
            {
                break;
            }
        }
    }
}

void FileMonitor::handleInotifyParentDirEvent(const inotify_event* parentDirEvent)
{
    const bool fileNameSet = parentDirEvent->len > 1;

    if (!fileNameSet)
    {
        LOG(DEBUG, "handleInotifyParentDirEvent : {}, no file name.", parentDirEvent->mask);
        return;
    }

    const std::filesystem::path fileName(parentDirEvent->name);

    LOG(DEBUG, "handleInotifyParentDirEvent : {}, file name: {}",
        parentDirEvent->mask, fileName.string());

    if (fileName != m_path.filename())
    {
        return;
    }

    if (parentDirEvent->mask & IN_MOVED_FROM)
    {
        LOG(DEBUG, "File moved out (deletion) detected.");

        inotify_rm_watch(m_inotifyFileDesc, m_fileWatchDesc);
        m_fileWatchDesc = INVALID_FD;

        m_fileState = PendingDeletion;
    }

    if ((parentDirEvent->mask & IN_MOVED_TO) || (parentDirEvent->mask & IN_CREATE))
    {
        if (std::filesystem::is_regular_file(m_path))
        {
            if (m_fileWatchDesc != INVALID_FD)
            {
                // this can happen e.g. if original file is replaced inplace
                // by temporary file (e.g rename(".foo.tmp", "foo"))
                LOG(DEBUG, "The file {} replaced in place.", fileName.string());

                inotify_rm_watch(m_inotifyFileDesc, m_fileWatchDesc);
            }
            else
            {
                LOG(DEBUG, "The file {} move to/creation in parent dir. detected.",
                    fileName.string());
            }

            m_fileWatchDesc = inotify_add_watch(m_inotifyFileDesc, m_path.c_str(),
                                                IN_MODIFY | IN_DELETE_SELF);

            if (m_fileWatchDesc == INVALID_FD)
            {
                LOG(ERROR, "Cannot add inotify watch for {}", m_path.string());
                throw std::runtime_error("inotify_add_watch failure");
            }

            m_fileState = PendingModification;
        }
    }
}

void FileMonitor::handleInotifyFileEvent(const inotify_event* fileEvent)
{

    LOG(DEBUG, "handleFileEvent : {}", fileEvent->mask);

    if (fileEvent->mask & IN_MODIFY)
    {
        LOG(DEBUG, "File modification detected.");

        m_fileState = PendingModification;
    }

    if (fileEvent->mask & IN_DELETE_SELF)
    {
        LOG(DEBUG, "File deletion detected.");

        inotify_rm_watch(m_inotifyFileDesc, m_fileWatchDesc);
        m_fileWatchDesc = INVALID_FD;

        m_fileState = PendingDeletion;
    }
}

void FileMonitor::commitFileEvent()
{
    if (m_fileState == PendingModification)
    {
        LOG(INFO, "File modification was reported.");
        m_observer.handleFileEvent(FileObserverI::FileModified);

        // re-starts monitoring cycle
        m_fileState = NoChangesDetected;
    }
    else if (m_fileState == PendingDeletion)
    {
        LOG(INFO, "File deletion was reported.");
        m_observer.handleFileEvent(FileObserverI::FileDeleted);

        // stops monitoring loop
        m_fileState = Deleted;
    }
}