#include "MockFileMonitorFactory.h"

#include <FileMonitorI.h>

namespace
{
FileMonitorI::FileMonitorFactory_t originalFactory;
} // namespace

void useFileMonitorFactoryMock(FileMonitorFactoryMock_t& factoryMock)
{
    originalFactory = FileMonitorI::create;
    FileMonitorI::create = factoryMock.AsStdFunction();
}

void restoreFileMonitorFactory()
{
    FileMonitorI::create = originalFactory;
}
