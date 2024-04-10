#include "ChangeToProtoMsgConverter.h"

#include <ApWatchI.pb.h>

namespace
{
void toProtoBufType(const AccessPoint& changeAp, ApWatchI::AccessPoint* protoAp)
{
    protoAp->set_ssid(changeAp.SSID);
    protoAp->set_snr(changeAp.SNR);
    protoAp->set_channel(changeAp.channel);
}

ApWatchI::ModifiedApParamsMsg::Param toProtoBufType(ModifiedApParamsChange::Param param)
{
    switch (param)
    {
    case ModifiedApParamsChange::SNR:
        return ApWatchI::ModifiedApParamsMsg::SNR;
    case ModifiedApParamsChange::channnel:
        return ApWatchI::ModifiedApParamsMsg::channnel;
    default:
        return ApWatchI::ModifiedApParamsMsg::None;
    }
}
} // namespace

msg::MsgDescriptor ChangeToProtoMsgConverter::operator()(const NewApChange& change)
{
    ApWatchI::NewApMsg msg;

    toProtoBufType(change.newAP, msg.mutable_ap());

    return msg::createMsgDescriptor(msg::IfaceId::ApWatchI, ApWatchI::NewAp, msg);
}

msg::MsgDescriptor ChangeToProtoMsgConverter::operator()(const RemovedApChange& change)
{
    ApWatchI::RemovedApMsg msg;

    toProtoBufType(change.oldAP, msg.mutable_ap());

    return msg::createMsgDescriptor(msg::IfaceId::ApWatchI, ApWatchI::RemovedAp, msg);
}

msg::MsgDescriptor ChangeToProtoMsgConverter::operator()(const ModifiedApParamsChange& change)
{
    ApWatchI::ModifiedApParamsMsg msg;

    toProtoBufType(change.oldAP, msg.mutable_oldap());
    toProtoBufType(change.newAP, msg.mutable_newap());

    for (const auto param : change.changedParams)
    {
        msg.add_changedparams(toProtoBufType(param));
    }

    return msg::createMsgDescriptor(msg::IfaceId::ApWatchI, ApWatchI::ModifiedApParams, msg);
}
