#pragma once

#include <string>
#include <vector>

namespace BGLRenderer
{
    class ConsoleWindow
    {
    public:
        ConsoleWindow();
        ~ConsoleWindow();
        
        void onIMGUI();

        void write(const std::string& message);

    private:
        std::vector<std::string> _messages;
        bool _scroll_to_bottom;
    };
}
