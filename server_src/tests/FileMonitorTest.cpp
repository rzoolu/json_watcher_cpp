#include <FileMonitorI.h>

#include "mocks/MockFileObserver.h"

#include <fcntl.h>
#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <thread>
#include <unistd.h>

namespace
{

using std::chrono::milliseconds;

template <typename... Ops>
void asyncDelayedOperations(const std::filesystem::path& file,
                            milliseconds delay,
                            Ops&&... operations)
{
    std::this_thread::sleep_for(delay);

    // Run every operation, followed be thread sleep
    ((operations(file), std::this_thread::sleep_for(delay)), ...);
}

template <typename... Ops>
auto scheduleDeleyedFileOperations(const std::filesystem::path& file,
                                   milliseconds delay,
                                   Ops... operations)
{
    std::thread t(asyncDelayedOperations<Ops...>,
                  std::cref(file),
                  std::move(delay),
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

void replaceFile(const std::filesystem::path& file)
{
    std::cerr << "UT replaceFile \n";

    auto swpFile = file;
    swpFile += ".swp";

    auto fd = creat(swpFile.c_str(), S_IRUSR | S_IWUSR);

    constexpr char data[] = "_initial_data_";

    write(fd, data, sizeof(data));
    fsync(fd);
    close(fd);

    rename(swpFile.c_str(), file.c_str());
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

    auto fileOpsThread = scheduleDeleyedFileOperations(m_tmpFilePath, milliseconds(100), deleteFile);

    fileMonitor->startMonitoring(FileMonitorI::NO_STABILIZATION);
    fileOpsThread.join();
}

TEST_F(FileMonitorTest, observerIsNotifiedOnceWhenFileIsModifiedAndDeletedWithinStabilzationPeriod)
{
    MockFileObserver mockFileObs;

    EXPECT_CALL(mockFileObs, handleFileEvent(FileObserverI::FileModified)).Times(0);

    EXPECT_CALL(mockFileObs, handleFileEvent(FileObserverI::FileDeleted)).Times(1);

    auto fileMonitor = FileMonitorI::create(m_tmpFilePath, mockFileObs);

    auto fileOpsThread = scheduleDeleyedFileOperations(m_tmpFilePath, milliseconds(90),
                                                       modifyFile, modifyFile, deleteFile);

    const auto stabilityTimeout = milliseconds(200);
    fileMonitor->startMonitoring(stabilityTimeout);
    fileOpsThread.join();
}

TEST_F(FileMonitorTest, observerIsNotifiedWhenFileIsModified)
{
    MockFileObserver mockFileObs;

    EXPECT_CALL(mockFileObs, handleFileEvent(FileObserverI::FileModified));

    // delete to stop monitoring
    EXPECT_CALL(mockFileObs, handleFileEvent(FileObserverI::FileDeleted));

    auto fileMonitor = FileMonitorI::create(m_tmpFilePath, mockFileObs);

    auto fileOpsThread = scheduleDeleyedFileOperations(m_tmpFilePath, milliseconds(100),
                                                       modifyFile, deleteFile);

    fileMonitor->startMonitoring(FileMonitorI::NO_STABILIZATION);
    fileOpsThread.join();
}

TEST_F(FileMonitorTest, observerIsNotifiedWhenFileIsModifiedByReplacement)
{
    MockFileObserver mockFileObs;

    EXPECT_CALL(mockFileObs, handleFileEvent(FileObserverI::FileModified));

    // delete to stop monitoring
    EXPECT_CALL(mockFileObs, handleFileEvent(FileObserverI::FileDeleted));

    auto fileMonitor = FileMonitorI::create(m_tmpFilePath, mockFileObs);

    auto fileOpsThread = scheduleDeleyedFileOperations(m_tmpFilePath, milliseconds(100),
                                                       replaceFile, deleteFile);

    fileMonitor->startMonitoring(FileMonitorI::NO_STABILIZATION);
    fileOpsThread.join();
}

TEST_F(FileMonitorTest, observerIsNotifiedWhenFileIsModifiedMultipleTimes)
{
    MockFileObserver mockFileObs;

    EXPECT_CALL(mockFileObs, handleFileEvent(FileObserverI::FileModified)).Times(3);

    // delete to stop monitoring
    EXPECT_CALL(mockFileObs, handleFileEvent(FileObserverI::FileDeleted));

    auto fileMonitor = FileMonitorI::create(m_tmpFilePath, mockFileObs);

    auto fileOpsThread = scheduleDeleyedFileOperations(m_tmpFilePath, milliseconds(100),
                                                       modifyFile, modifyFile, modifyFile, deleteFile);

    fileMonitor->startMonitoring(FileMonitorI::NO_STABILIZATION);
    fileOpsThread.join();
}