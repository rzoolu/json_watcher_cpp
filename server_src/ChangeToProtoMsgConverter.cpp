#include <ChangeToProtoMsgConverter.h>

#include <ApWatchI.pb.h>

namespace
{
void changedAPToProtoAP(const AccessPoint& changeAp, ApWatchI::AccessPoint* protoAp)
{
    protoAp->set_ssid(changeAp.SSID);
    protoAp->set_snr(changeAp.SNR);
    protoAp->set_channel(changeAp.channel);
}

} // namespace

std::string ChangeToProtoMsgConverter::operator()(const NewApChange& change)
{
    ApWatchI::Msg msg;

    changedAPToProtoAP(change.newAP, msg.mutable_newap()->mutable_ap());

    return msg.SerializeAsString();
}

std::string ChangeToProtoMsgConverter::operator()(const RemovedApChange& change)
{
    ApWatchI::Msg msg;

    changedAPToProtoAP(change.oldAP, msg.mutable_removedap()->mutable_ap());

    return msg.SerializeAsString();
}

std::string ChangeToProtoMsgConverter::operator()(const ModifiedApParamsChange& change)
{
    ApWatchI::Msg msg;

    changedAPToProtoAP(change.oldAP, msg.mutable_modifiedap()->mutable_oldap());
    changedAPToProtoAP(change.newAP, msg.mutable_modifiedap()->mutable_newap());

    return msg.SerializeAsString();
}
