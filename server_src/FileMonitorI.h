#pragma once

#include <filesystem>
#include <functional>
#include <memory>

class FileObserverI
{
public:
    enum Event
    {
        FileModified,
        FileDeleted
    };

    virtual void handleFileEvent(Event event);
};

class FileMonitorI
{
public:
    using FileMonitorFactory_t =
        std::function<std::unique_ptr<FileMonitorI>(const std::filesystem::path& file,
                                                    FileObserverI& observer)>;

    static FileMonitorFactory_t create;

    virtual ~FileMonitorI() = default;

    virtual void startMonitoring() = 0;
};
