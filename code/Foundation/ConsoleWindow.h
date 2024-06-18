#pragma once

#include <string>
#include <vector>

#include "Log.h"

namespace BGLRenderer
{
    class ConsoleWindow
    {
    public:
        ConsoleWindow();
        ~ConsoleWindow();
        
        void onIMGUI(bool& showConsole);

        void write(const LogMessage& message);

    private:
        std::vector<LogMessage> _messages;
        bool _scrollToBottom = false;
        bool _showErrorsOnly = false;
    };
}
