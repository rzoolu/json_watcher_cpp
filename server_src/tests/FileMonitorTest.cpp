#include <FileMonitorI.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

using testing::InvokeWithoutArgs;

class MockFileObserverI : public FileObserverI
{
public:
    MOCK_METHOD(void, handleFileEvent, (FileObserverI::Event), (override));
};

namespace
{
void modifyFile(const std::filesystem::path& file)
{
    std::ofstream ofs(file);
    ofs << "append_data";
}
} // namespace

class FileMonitorTest : public testing::Test
{
protected:
    FileMonitorTest() : m_tmpFilePath("./tmp"){};

    void deleteFile()
    {
        std::filesystem::remove(m_tmpFilePath);
    }

    const std::filesystem::path m_tmpFilePath;
};

// TODO:
// TEST_F(FileMonitorTest, observerIsNotifiedWhenFileIsModified)
// {
//     MockFileObserverI mockFileObs;

//     EXPECT_CALL(mockFileObs, handleFileEvent(FileObserverI::FileModified))
//         .WillOnce(InvokeWithoutArgs([this]()
//                                     { deleteFile(); }));

//     auto fileMonitor = FileMonitorI::create(m_tmpFilePath, mockFileObs);
//     fileMonitor->startMonitoring();

//     modifyFile(m_tmpFilePath);
// }
