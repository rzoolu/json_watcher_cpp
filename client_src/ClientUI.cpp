#include "ClientUI.h"

#include <ftxui/screen/screen.hpp>

#include <algorithm>
#include <chrono>
#include <format>
#include <iostream>
#include <string>

using namespace ftxui;

ClientUI& ClientUI::getInstance()
{
    static ClientUI ui;

    return ui;
}

void ClientUI::draw()
{
    constexpr auto MinimumScreenWidth = 60u;

    auto header = createHeader();
    auto msgElement = createMessagesElement();
    auto footer = createFooter();

    auto allContent = borderRounded(vbox({std::move(header),
                                          separator(),
                                          std::move(msgElement),
                                          separator(),
                                          std::move(footer)})) |
                      size(WIDTH, GREATER_THAN, MinimumScreenWidth);

    auto screen = Screen::Create(Dimension::Fit(allContent), Dimension::Fit(allContent));
    Render(screen, allContent);
    std::cout << m_resetPosition;
    screen.Print();
    m_resetPosition = screen.ResetPosition(true);
}

void ClientUI::drawMessage(const ApWatchI::NewApMsg& newApMsg)
{

    auto fields = vbox({hbox({text("Timestamp: "), text(getTimeStamp())}),
                        hbox({text("SSID: "), text(newApMsg.ap().ssid()) | bold}),
                        hbox({text(" SNR: "), text(std::to_string(newApMsg.ap().snr()))}),
                        hbox({text(" channel: "), text(std::to_string(newApMsg.ap().channel()))})}) |
                  color(Color::Default);

    auto msg = window(text(" New Ap Message "), fields) | color(Color::Green);
    drawMessage(std::move(msg));
}

void ClientUI::drawMessage(const ApWatchI::RemovedApMsg& removedApMsg)
{
    auto fields = vbox({hbox({text("Timestamp: "), text(getTimeStamp())}),
                        hbox({text("SSID: "), text(removedApMsg.ap().ssid()) | bold}),
                        hbox({text(" SNR: "), text(std::to_string(removedApMsg.ap().snr()))}),
                        hbox({text(" channel: "), text(std::to_string(removedApMsg.ap().channel()))})}) |
                  color(Color::Default);

    auto msg = window(text(" Removed Ap Message "), fields) | color(Color::Red);
    drawMessage(std::move(msg));
}

void ClientUI::drawMessage(const ApWatchI::ModifiedApParamsMsg& modifiedApMsg)
{

    Elements vboxElements{hbox({text("Timestamp: "), text(getTimeStamp())}),
                          hbox({text("SSID: "), text(modifiedApMsg.newap().ssid()) | bold})};

    const auto& changedParams = modifiedApMsg.changedparams();
    for (const auto& param : changedParams)
    {
        switch (param)
        {
        case ApWatchI::ModifiedApParamsMsg::SNR:
            vboxElements.push_back(hbox({text(" SNR changed: "),
                                         text(std::to_string(modifiedApMsg.oldap().snr())),
                                         text(" -> "),
                                         text(std::to_string(modifiedApMsg.newap().snr()))}));

            break;
        case ApWatchI::ModifiedApParamsMsg::channnel:
            vboxElements.push_back(hbox({text(" channel changed: "),
                                         text(std::to_string(modifiedApMsg.oldap().channel())),
                                         text(" -> "),
                                         text(std::to_string(modifiedApMsg.newap().channel()))}));
            break;

        default:
            break;
        }
    }

    auto changedFields = vbox(vboxElements) | color(Color::Default);

    auto msg = window(text(" Changed  Ap Message "), changedFields) | color(Color::Yellow);

    drawMessage(std::move(msg));
}

void ClientUI::drawMessage(Element msg)
{
    constexpr auto LastMessagesMaxSize = 5u;

    if (m_lastMessages.size() == LastMessagesMaxSize)
    {
        m_lastMessages.pop_back();
    }

    m_lastMessages.push_front(std::move(msg));

    draw();
}

ftxui::Element ClientUI::createHeader()
{
    return hcenter(text("Last messages: ")) | bold;
}

ftxui::Element ClientUI::createMessagesElement()
{
    Element element;

    if (m_lastMessages.empty())
    {
        element = center(text("(No messages)"));
    }
    else
    {
        Elements messages;

        std::copy(m_lastMessages.begin(),
                  m_lastMessages.end(),
                  std::back_inserter(messages));

        element = hbox(std::move(messages));
    }

    constexpr auto MiniumMessagesElementHeight = 6u;
    return element | size(HEIGHT, GREATER_THAN, MiniumMessagesElementHeight);
}

ftxui::Element ClientUI::createFooter()
{
    return text("Press Ctrl^C to exit.");
}

std::string ClientUI::getTimeStamp()
{
    const auto nowInOurZone{std::chrono::zoned_time{
        std::chrono::current_zone(),
        std::chrono::system_clock::now()}};

    // formatted date and time in current zone
    return std::format("{:%x %X}", nowInOurZone);
}