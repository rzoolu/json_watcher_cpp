#pragma once

#include <chrono>
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

    virtual void handleFileEvent(Event event) = 0;

protected:
    ~FileObserverI() = default;
};

class FileMonitorI
{
public:
    using FileMonitorFactory_t =
        std::function<std::unique_ptr<FileMonitorI>(const std::filesystem::path& file,
                                                    FileObserverI& observer)>;

    static FileMonitorFactory_t create;

    virtual ~FileMonitorI() = default;

    // In many text editors (e.g vim, gedit, etc.) simple file write after edition
    // results in fast sequence of file operations/events (e.g. tmp/swp file is created,
    // then original file is removed, and finally created again with new/edited content).
    // Similary simple cat "new.json" > "monitored.json" can result in fast sequence
    // of 'modify' events.
    //
    // To filter-out those cases, and avoid redudndat file content parsing (or missleading
    // 'delete' events) stabiltyTimeOut can be used.
    // E.g. when stabiltyTimeOut = 50 ms, monitor will call FileObserverI::handleFileEvent
    // when file was modifed or deleted at least once, and at least 50 ms has passed since
    // last operation. Seqence of delete/re-create within stabiltyTimeOut are reported as
    // modification.
    // When stabiltyTimeOut < 1 ms, then every event is delivered immediately to observer.
    //
    // Monitored file has to exists while calling startMonitoring().
    // This function returns only when file is deleted (after FileDeleted was
    // reported), otherwise it will keep running forever.

    // Constant to be used when events should be deliverd without stabilization.
    static constexpr std::chrono::milliseconds NO_STABILIZATION{0};

    virtual void startMonitoring(std::chrono::milliseconds stabiltyTimeOut) = 0;
};
