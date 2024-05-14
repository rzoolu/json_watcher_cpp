#include "ClientApp.h"

#include "ApWatchIMsgHandler.h"

#include <Message.h>
#include <MessageSubscriberI.h>

constexpr auto APP_TCP_PORT = 8282;

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

namespace
{

using namespace ftxui;

void drawWithFtxUi()
{
    auto summary = []
    {
        auto content = vbox({
            hbox({text(L"- field1: "), text(L"value1") | bold}) | color(Color::Green),
            hbox({text(L"- field2: "), text(L"2") | bold}) | color(Color::RedLight),
            hbox({text(L"- field3  "), text(L"3") | bold}) | color(Color::Red),
        });
        return window(text(L" Message "), content);
    };

    auto document = vbox({window(text(L"This is test"), text(L" T E S T ")) | flex,
                          hbox({summary(), summary(), summary()})});

    // Limit the size of the document to 80 char.
    document = document | size(WIDTH, LESS_THAN, 80);

    auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    Render(screen, document);

    std::cout << screen.ToString() << '\0' << std::endl;
}
} // namespace

void ClientApp::run()
{
    ApWatchIMsgHandler apWatchMsgHandler;

    auto msgSub = msg::MessageSubscriberI::create("127.0.0.1",
                                                  APP_TCP_PORT,
                                                  msg::IfaceId::ApWatchI,
                                                  apWatchMsgHandler);

    drawWithFtxUi();

    msgSub->startReceiving();
}
