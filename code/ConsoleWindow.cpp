#include "ConsoleWindow.h"

#include <imgui.h>

#include "Log.h"

namespace BGLRenderer
{
    ConsoleWindow::ConsoleWindow()
    {
        Log::listen([&](const std::string& message)
        {
            write(message);
        });
    }

    ConsoleWindow::~ConsoleWindow()
    {
        // TODO: remove listener
    }

    void ConsoleWindow::onIMGUI()
    {
        ImGui::Begin("Console");

        if (ImGui::Button("Clear"))
        {
            _messages.clear();
        }

        if (ImGui::BeginChild("ScrollRegion##"))
        {
            for (auto& message : _messages)
            {
                ImGui::TextUnformatted(message.c_str());
            }

            if (_scroll_to_bottom)
            {
                ImGui::SetScrollHereY(1.0f);
                _scroll_to_bottom = false;
            }
        }
        ImGui::EndChild();

        ImGui::End();
    }

    void ConsoleWindow::write(const std::string& message)
    {
        _messages.push_back(message);
    }
}
