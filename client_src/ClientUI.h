#pragma once

#include <ApWatchI.pb.h>
#include <ftxui/dom/elements.hpp>

#include <deque>

class ClientUI
{
public:
    static ClientUI& getInstance();

    void draw();

    void drawMessage(const ApWatchI::NewApMsg& newApMsg);
    void drawMessage(const ApWatchI::RemovedApMsg& removedApMsg);
    void drawMessage(const ApWatchI::ModifiedApParamsMsg& modifiedApMsg);

protected:
    ClientUI() = default;
    void drawMessage(ftxui::Element msg);

private:
    ftxui::Element createHeader();
    ftxui::Element createMessagesElement();
    ftxui::Element createFooter();
    std::string getTimeStamp();

private:
    std::deque<ftxui::Element> m_lastMessages;
    std::string m_resetPosition;
};