#pragma once

#include <string>
#include <format>
#include <functional>

namespace BGLRenderer
{
    constexpr const char* DebugLogPrefix = "DEBUG";
    constexpr const char* ErrorLogPrefix = "ERROR";
    constexpr const char* WarningLogPrefix = "WARNING";

    using LogListenerFn = std::function<void(const std::string&)>;

    class Log
    {
    public:
        Log(const std::string& category);

        inline void debug(const std::string& message) { write(getLogMessagePrefix(DebugLogPrefix, _category.c_str()) + message); }

        template <class... Args>
        void debug(const std::string& message, Args&&... args)
        {
            write(getLogMessagePrefix(DebugLogPrefix, _category.c_str()) + std::vformat(message, std::make_format_args(std::forward<Args>(args)...)));
        }

        inline void warning(const std::string& message) { write(getLogMessagePrefix(WarningLogPrefix, _category.c_str()) + message); }

        template <class... Args>
        void warning(const std::string& message, Args&&... args)
        {
            write(getLogMessagePrefix(WarningLogPrefix, _category.c_str()) + std::vformat(message, std::make_format_args(std::forward<Args>(args)...)));
        }

        inline void error(const std::string& message) { write(getLogMessagePrefix(ErrorLogPrefix, _category.c_str()) + message); }

        template <class... Args>
        void error(const std::string& message, Args&&... args)
        {
            write(getLogMessagePrefix(ErrorLogPrefix, _category.c_str()) + std::vformat(message, std::make_format_args(std::forward<Args>(args)...)));
        }

        static void listen(const LogListenerFn& listener);

    private:
        std::string _category;

        void write(const std::string& message);

        static std::string getLogMessagePrefix(const char* severenity, const char* category)
        {
            constexpr std::size_t desiredWidth = 24;
            std::string prefix = std::format("[{}][{}] ", severenity, category);

            if (prefix.length() < desiredWidth)
            {
                prefix += std::string(desiredWidth - prefix.length(), ' ');
            }

            return prefix;
        }
    };
}
