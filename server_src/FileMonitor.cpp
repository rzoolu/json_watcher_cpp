#include <FileMonitor.h>

FileMonitorI::FileMonitorFactory_t FileMonitorI::create = [](const std::filesystem::path& file,
                                                             FileObserverI& observer)
{
    return std::make_unique<FileMonitor>(file, observer);
};

FileMonitor::FileMonitor(const std::filesystem::path& file, FileObserverI& observer)
    : m_path(file),
      m_observer(observer)

{
}

void FileMonitor::startMonitoring()
{
}