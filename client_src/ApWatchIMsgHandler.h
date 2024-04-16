#pragma once

#include <MessageHandlerI.h>

class ApWatchIMsgHandler : public msg::MessageHandlerI
{
public:
    AfterHandleAction handleMessage(const msg::MsgDescriptor& msg) override;
};
