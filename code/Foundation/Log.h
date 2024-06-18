#pragma once

#include <string>
#include <format>
#include <functional>

namespace BGLRenderer
{
    constexpr const char* DebugLogPrefix = "DEBUG";
    constexpr const char* ErrorLogPrefix = "ERROR";
    constexpr const char* WarningLogPrefix = "WARNING";

    enum class LogSeverity
    {
        debug,
        warning,
        error
    };

    constexpr const char* logSeverityToCString(LogSeverity severity)
    {
        if (severity == LogSeverity::debug)
        {
            return DebugLogPrefix;
        }
        else if (severity == LogSeverity::warning)
        {
            return WarningLogPrefix;
        }
        else if (severity == LogSeverity::error)
        {
            return ErrorLogPrefix;
        }

        static_assert(true, "Unsupported severity value");
        return "unknown";
    }

    struct LogMessage
    {
        LogSeverity severity;
        std::string category;
        std::string message;
    };

    using LogListenerFn = std::function<void(const LogMessage&)>;

    class Log
    {
    public:
        Log(const std::string& category);

        inline void debug(const std::string& message) { write({LogSeverity::debug, _category, message}); }

        template <class... Args>
        void debug(const std::string& message, Args&&... args)
        {
            write({LogSeverity::debug, _category, std::vformat(message, std::make_format_args(std::forward<Args>(args)...))});
        }

        inline void warning(const std::string& message) { write({LogSeverity::warning, _category, message}); }

        template <class... Args>
        void warning(const std::string& message, Args&&... args)
        {
            write({LogSeverity::warning, _category, std::vformat(message, std::make_format_args(std::forward<Args>(args)...))});
        }

        inline void error(const std::string& message) { write({LogSeverity::error, _category, message}); }

        template <class... Args>
        void error(const std::string& message, Args&&... args)
        {
            write({LogSeverity::error, _category, std::vformat(message, std::make_format_args(std::forward<Args>(args)...))});
        }

        static void listen(const LogListenerFn& listener);

    private:
        std::string _category;

        void write(const LogMessage& message);

    };

    namespace LogUtils
    {
        std::string getLogMessagePrefix(const char* severity, const char* category);
    }
}
