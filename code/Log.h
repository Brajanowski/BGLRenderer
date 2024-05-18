#pragma once

#include <string>
#include <format>
#include <functional>

namespace BGLRenderer
{
    constexpr const char* DebugLogPrefix = "[DEBUG]   ";
    constexpr const char* ErrorLogPrefix = "[ERROR]   ";
    constexpr const char* WarningLogPrefix = "[WARNING] ";

    typedef std::function<void(const std::string&)> LogListenerFn;

    class Log
    {
    public:
        Log(const std::string& category);

        inline void debug(const std::string& message) { write(DebugLogPrefix + message); }

        template <class... Args>
        void debug(const std::string& message, Args&&... args)
        {
            write(DebugLogPrefix + std::vformat(message, std::make_format_args(std::forward<Args>(args)...)));
        }

        inline void warning(const std::string& message) { write(WarningLogPrefix + message); }

        template <class... Args>
        void warning(const std::string& message, Args&&... args)
        {
            write(WarningLogPrefix + std::vformat(message, std::make_format_args(std::forward<Args>(args)...)));
        }

        inline void error(const std::string& message) { write(ErrorLogPrefix + message); }

        template <class... Args>
        void error(const std::string& message, Args&&... args)
        {
            write(ErrorLogPrefix + std::vformat(message, std::make_format_args(std::forward<Args>(args)...)));
        }

        static void listen(const LogListenerFn& listener);

    private:
        std::string _category;

        void write(const std::string& message);
    };
}
