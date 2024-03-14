#include <FileMonitorI.h>

#include "mocks/MockFileObserver.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <thread>

#include <fcntl.h>
#include <unistd.h>

namespace
{

template <typename... Ops>
void asyncDelayedOperations(const std::filesystem::path& file, Ops&&... operations)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    ((operations(file), std::chrono::milliseconds(50)), ...);
}

template <typename... Ops>
auto scheduleDeleyedFileOperations(const std::filesystem::path& file, Ops... operations)
{
    std::thread t(asyncDelayedOperations<Ops...>,
                  std::cref(file),
                  std::move(operations)...);

    return t;
}

// POSIX unbuffered direct file manipulation functions
void modifyFile(const std::filesystem::path& file)
{
    std::cerr << "UT modifyFile \n";

    auto fd = open(file.c_str(), O_WRONLY | O_APPEND);

    constexpr char data[] = "_appended_data_";

    write(fd, data, sizeof(data));
    fsync(fd);
    close(fd);
}

void deleteFile(const std::filesystem::path& file)
{
    std::cerr << "UT deleteFile \n";

    unlink(file.c_str());
}

void createFile(const std::filesystem::path& file)
{
    std::cerr << "UT createFile \n";

    auto fd = creat(file.c_str(), S_IRUSR | S_IWUSR);

    constexpr char data[] = "_initial_data_";

    write(fd, data, sizeof(data));
    fsync(fd);
    close(fd);
}

} // namespace

class FileMonitorTest : public testing::Test
{
protected:
    FileMonitorTest() : m_tmpFilePath("./tmp")
    {
        createFile(m_tmpFilePath);
    };

    ~FileMonitorTest()
    {
        deleteFile(m_tmpFilePath);
    }

    const std::filesystem::path m_tmpFilePath;
};

TEST_F(FileMonitorTest, observerIsNotifiedWhenFileIsDeleted)
{
    MockFileObserver mockFileObs;

    EXPECT_CALL(mockFileObs, handleFileEvent(FileObserverI::FileDeleted));

    auto fileMonitor = FileMonitorI::create(m_tmpFilePath, mockFileObs);

    auto fileOpsThread = scheduleDeleyedFileOperations(m_tmpFilePath, deleteFile);

    fileMonitor->startMonitoring();
    fileOpsThread.join();
}

TEST_F(FileMonitorTest, observerIsNotifiedWhenFileIsModified)
{
    MockFileObserver mockFileObs;

    EXPECT_CALL(mockFileObs, handleFileEvent(FileObserverI::FileModified));

    // delete to stop monitoring
    EXPECT_CALL(mockFileObs, handleFileEvent(FileObserverI::FileDeleted));

    auto fileMonitor = FileMonitorI::create(m_tmpFilePath, mockFileObs);

    auto fileOpsThread = scheduleDeleyedFileOperations(m_tmpFilePath, modifyFile, deleteFile);

    fileMonitor->startMonitoring();
    fileOpsThread.join();
}

TEST_F(FileMonitorTest, observerIsNotifiedWhenFileIsModifiedMultipleTimes)
{
    MockFileObserver mockFileObs;

    EXPECT_CALL(mockFileObs, handleFileEvent(FileObserverI::FileModified)).Times(3);

    // delete to stop monitoring
    EXPECT_CALL(mockFileObs, handleFileEvent(FileObserverI::FileDeleted));

    auto fileMonitor = FileMonitorI::create(m_tmpFilePath, mockFileObs);

    auto fileOpsThread = scheduleDeleyedFileOperations(m_tmpFilePath,
                                                       modifyFile, modifyFile, modifyFile, deleteFile);

    fileMonitor->startMonitoring();
    fileOpsThread.join();
}