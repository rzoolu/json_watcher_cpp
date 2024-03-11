#pragma once

#include <AccessPointsDataI.h>

#include <string>

struct ChangeToProtoMsgConverter
{
    std::string operator()(const NewApChange& change);
    std::string operator()(const RemovedApChange& change);
    std::string operator()(const ModifiedApParamsChange& change);
};