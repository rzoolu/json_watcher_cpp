#pragma once

#include <MessageHandlerI.h>

class ApWatchIMsgHandler : public msg::MessageHandlerI
{
public:
    void handleMessage(const msg::MsgDescriptor& msg) override;
};
