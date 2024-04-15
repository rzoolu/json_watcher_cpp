#pragma once

#include "AccessPointsDataI.h"

#include <Message.h>

#include <string>

struct ChangeToProtoMsgConverter
{
    msg::MsgDescriptor operator()(const NewApChange& change);
    msg::MsgDescriptor operator()(const RemovedApChange& change);
    msg::MsgDescriptor operator()(const ModifiedApParamsChange& change);
};
