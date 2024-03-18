#include "ClientApp.h"
#include "MessageSubscriberI.h"

constexpr auto APP_TCP_PORT = 8282;

void ClientApp::run()
{
    auto msgSub = MessageSubscriberI::create("127.0.0.1", APP_TCP_PORT);

    msgSub->startReceiving();
}
