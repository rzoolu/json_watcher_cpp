#pragma once

#include <gmock/gmock.h>

class MessagePublisherI;

using MessagePublisherFactoryMock_t =
    testing::MockFunction<std::unique_ptr<MessagePublisherI>(std::uint16_t)>;

void useMessagePublisherFactoryMock(MessagePublisherFactoryMock_t&);
void restoreMessagePublisherFactory();