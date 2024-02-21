#include "MockMessagePublisherFactory.h"

#include <MessagePublisherI.h>

namespace
{
MessagePublisherI::MessagePublisherFactory_t originalFactory;
} // namespace

void useMessagePublisherFactoryMock(MessagePublisherFactoryMock_t& factoryMock)
{
    originalFactory = MessagePublisherI::create;
    MessagePublisherI::create = factoryMock.AsStdFunction();
}

void restoreMessagePublisherFactory()
{
    MessagePublisherI::create = originalFactory;
}
