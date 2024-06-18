#include "ConsoleWindow.h"

#include <imgui.h>

#include "Log.h"

namespace BGLRenderer
{
    namespace Theme
    {
        static ImVec4 getMessageColorForSeverity(LogSeverity severity)
        {
            if (severity == LogSeverity::warning)
            {
                return {0.98f, 0.72f, 0.001f, 1.0f};
            }
            else if (severity == LogSeverity::error)
            {
                return {0.9f, 0.1f, 0.19f, 1.0f};
            }

            return {0.9f, 0.9f, 0.9f, 1};
        }
    }

    ConsoleWindow::ConsoleWindow()
    {
        Log::listen([&](const LogMessage& message)
        {
            write(message);
        });
    }

    ConsoleWindow::~ConsoleWindow()
    {
        // TODO: remove listener
    }

    void ConsoleWindow::onIMGUI(bool& showConsole)
    {
        // FIXME:
        static char filterBuffer[256] = {};

        if (ImGui::Begin("Console", &showConsole))
        {
            if (ImGui::Button("Clear"))
            {
                _messages.clear();
            }

            ImGui::SameLine();
            ImGui::InputText("Filter", filterBuffer, 256);

            ImGui::Checkbox("Show Errors Only", &_showErrorsOnly);

            std::string_view filterText = filterBuffer;

            if (ImGui::BeginChild("ScrollRegion##"))
            {
                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 100);

                for (std::size_t i = 0; i < _messages.size(); ++i)
                {
                    const LogMessage& logMessage = _messages[i];

                    if (_showErrorsOnly && logMessage.severity != LogSeverity::error)
                    {
                        continue;
                    }

                    if (filterText.empty() || logMessage.message.find(filterText) != std::string::npos)
                    {
                        ImGui::TextUnformatted(logMessage.category.c_str());

                        ImGui::NextColumn();

                        ImGui::PushStyleColor(ImGuiCol_Text, Theme::getMessageColorForSeverity(logMessage.severity));
                        ImGui::TextUnformatted(logMessage.message.c_str());
                        ImGui::PopStyleColor(); // Message text color

                        ImGui::NextColumn();
                    }
                }

                if (_scrollToBottom)
                {
                    ImGui::SetScrollHereY(1.0f);
                    _scrollToBottom = false;
                }

                ImGui::EndChild();
            }

        }

        ImGui::End();
    }

    void ConsoleWindow::write(const LogMessage& message)
    {
        _messages.push_back(message);
        _scrollToBottom = true;
    }
}
