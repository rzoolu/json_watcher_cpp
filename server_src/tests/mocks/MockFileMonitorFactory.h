#pragma once

#include <gmock/gmock.h>

#include <filesystem>

class FileMonitorI;
class FileObserverI;

using FileMonitorFactoryMock_t =
    testing::MockFunction<std::unique_ptr<FileMonitorI>(const std::filesystem::path&,
                                                        FileObserverI&)>;

void useFileMonitorFactoryMock(FileMonitorFactoryMock_t&);
void restoreFileMonitorFactory();