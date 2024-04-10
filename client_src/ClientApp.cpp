#include "ClientApp.h"

#include "ApWatchIMsgHandler.h"
#include "MessageSubscriberI.h"

#include <Messaging.h>

constexpr auto APP_TCP_PORT = 8282;

void ClientApp::run()
{
    ApWatchIMsgHandler apWatchMsgHandler;

    auto msgSub = MessageSubscriberI::create("127.0.0.1",
                                             APP_TCP_PORT,
                                             msg::IfaceId::ApWatchI,
                                             apWatchMsgHandler);

    msgSub->startReceiving();
}
