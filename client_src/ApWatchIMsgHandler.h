#pragma once

#include "MessageHandlerI.h"

class ApWatchIMsgHandler : public MessageHandlerI
{
public:
    void handleMessage(const msg::MsgDescriptor& msg) override;
};
