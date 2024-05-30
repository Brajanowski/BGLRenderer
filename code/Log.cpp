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

    void Log::write(const std::string& message)
    {
        for (const auto& listener : listeners)
        {
            listener(message);
        }
    }
}
