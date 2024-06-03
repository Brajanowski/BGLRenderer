#include "Log.h"

#include <vector>

namespace BGLRenderer
{
    static std::vector<LogListenerFn> listeners;
    
    Log::Log(const std::string& category) :
        _category(category)
    {
    }

    void Log::listen(const LogListenerFn& listener)
    {
        listeners.push_back(listener);
    }

    void Log::write(const LogMessage& message)
    {
        for (const auto& listener : listeners)
        {
            listener(message);
        }
    }
    
    namespace LogUtils
    {
        std::string getLogMessagePrefix(const char* severity, const char* category)
        {
            constexpr std::size_t desiredWidth = 24;
            std::string prefix = std::format("[{}][{}] ", severity, category);

            if (prefix.length() < desiredWidth)
            {
                prefix += std::string(desiredWidth - prefix.length(), ' ');
            }

            return prefix;
        }
    }
}
