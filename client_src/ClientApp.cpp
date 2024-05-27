#include "ClientApp.h"

#include "ApWatchIMsgHandler.h"
#include "ClientUI.h"

#include <Message.h>
#include <MessageSubscriberI.h>

constexpr msg::MessageSubscriberI::TcpPort APP_TCP_PORT = 8282;

void ClientApp::run()
{
    ApWatchIMsgHandler apWatchMsgHandler;

    auto msgSub = msg::MessageSubscriberI::create("127.0.0.1",
                                                  APP_TCP_PORT,
                                                  msg::IfaceId::ApWatchI,
                                                  apWatchMsgHandler);

    ClientUI::getInstance().draw();
    msgSub->startReceiving();
}
