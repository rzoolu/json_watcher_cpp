#include "ApWatchIMsgHandler.h"
#include "ClientUI.h"

#include <ApWatchI.pb.h>
#include <Log.h>
#include <Message.h>

msg::MessageHandlerI::AfterHandleAction
ApWatchIMsgHandler::handleMessage(const msg::MsgDescriptor& msg)
{

    if (msg.header.ifaceId == msg::IfaceId::ApWatchI)
    {
        auto& clientUi = ClientUI::getInstance();

        switch (msg.header.msgId)
        {
        case ApWatchI::NewAp:
        {
            ApWatchI::NewApMsg newApMsg;
            newApMsg.ParseFromString(msg.body);
            clientUi.drawMessage(newApMsg);

            LOG(DEBUG, "NewAp message: ssid is: {}", newApMsg.ap().ssid());
        }
        break;

        case ApWatchI::RemovedAp:
        {
            ApWatchI::RemovedApMsg removedApMsg;
            removedApMsg.ParseFromString(msg.body);
            clientUi.drawMessage(removedApMsg);

            LOG(DEBUG, "RemovedAp message, ssid is: {}", removedApMsg.ap().ssid());
        }
        break;

        case ApWatchI::ModifiedApParams:
        {
            ApWatchI::ModifiedApParamsMsg modifiedApMsg;
            modifiedApMsg.ParseFromString(msg.body);
            clientUi.drawMessage(modifiedApMsg);

            LOG(DEBUG, "ModifiedApParams message, ssid is: {}", modifiedApMsg.oldap().ssid());

            const auto& changedParams = modifiedApMsg.changedparams();

            for (auto param : changedParams)
            {
                switch (param)
                {
                case ApWatchI::ModifiedApParamsMsg::SNR:
                    LOG(DEBUG, "SNR has changed from {} to {}", modifiedApMsg.oldap().snr(), modifiedApMsg.newap().snr());
                    break;

                case ApWatchI::ModifiedApParamsMsg::channnel:
                    LOG(DEBUG, "channel has changed from {} to {}", modifiedApMsg.oldap().channel(), modifiedApMsg.newap().channel());
                    break;

                default:
                    break;
                }
            }
        }
        break;

        default:
            LOG(DEBUG, "Unintresting message: iface={}, msgId={}",
                msg::toStr(msg.header.ifaceId), msg.header.msgId);
            break;
        }
    }

    return MessageHandlerI::ContinueReceiving;
}
