#include <ChangeToProtoMsgConverter.h>

#include <ApWatchI.pb.h>

namespace
{
void toProtoBufType(const AccessPoint& changeAp, ApWatchI::AccessPoint* protoAp)
{
    protoAp->set_ssid(changeAp.SSID);
    protoAp->set_snr(changeAp.SNR);
    protoAp->set_channel(changeAp.channel);
}

ApWatchI::ModifiedApParams::Param toProtoBufType(ModifiedApParamsChange::Param param)
{
    switch (param)
    {
    case ModifiedApParamsChange::SNR:
        return ApWatchI::ModifiedApParams::SNR;
    case ModifiedApParamsChange::channnel:
        return ApWatchI::ModifiedApParams::channnel;
    default:
        return ApWatchI::ModifiedApParams::None;
    }
}
} // namespace

std::string ChangeToProtoMsgConverter::operator()(const NewApChange& change)
{
    ApWatchI::Msg msg;

    toProtoBufType(change.newAP, msg.mutable_newap()->mutable_ap());

    return msg.SerializeAsString();
}

std::string ChangeToProtoMsgConverter::operator()(const RemovedApChange& change)
{
    ApWatchI::Msg msg;

    toProtoBufType(change.oldAP, msg.mutable_removedap()->mutable_ap());

    return msg.SerializeAsString();
}

std::string ChangeToProtoMsgConverter::operator()(const ModifiedApParamsChange& change)
{
    ApWatchI::Msg msg;

    toProtoBufType(change.oldAP, msg.mutable_modifiedap()->mutable_oldap());
    toProtoBufType(change.newAP, msg.mutable_modifiedap()->mutable_newap());

    for (const auto param : change.changedParams)
    {
        msg.mutable_modifiedap()->add_changedparams(toProtoBufType(param));
    }

    return msg.SerializeAsString();
}
