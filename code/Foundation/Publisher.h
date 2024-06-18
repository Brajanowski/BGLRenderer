#pragma once

#include "Base.h"

#include <functional>
#include <map>

namespace BGLRenderer
{
    class PublisherEmpty
    {
    public:
        using CallbackFn = std::function<void()>;
        using ListenerHandle = std::size_t;

        static constexpr ListenerHandle listenerHandleInvalid = static_cast<ListenerHandle>(-1);

        void publish()
        {
            for (const auto& [handle, callback] : _listeners)
            {
                callback();
            }
        }

        [[nodiscard]] ListenerHandle listen(const CallbackFn& callback)
        {
            _listeners[_counter] = callback;
            _counter++;
            return _counter - 1;
        }

        void removeListener(ListenerHandle handle)
        {
            if (!_listeners.contains(handle))
            {
                return;
            }

            _listeners.erase(handle);
        }

        inline std::size_t listenersCount() const { return _listeners.size(); }
        
    private:
        std::map<ListenerHandle, CallbackFn> _listeners;
        ListenerHandle _counter = 0;
    };
    
    template<typename TEvent>
    class Publisher
    {
    public:
        using CallbackFn = std::function<void(const TEvent& ev)>;
        using ListenerHandle = std::size_t;

        static constexpr ListenerHandle listenerHandleInvalid = static_cast<ListenerHandle>(-1);

        void publish(const TEvent& ev)
        {
            for (const auto& [handle, callback] : _listeners)
            {
                callback(ev);
            }
        }

        [[nodiscard]] ListenerHandle listen(const CallbackFn& callback)
        {
            _listeners[_counter] = callback;
            _counter++;
            return _counter - 1;
        }

        void removeListener(ListenerHandle handle)
        {
            if (!_listeners.contains(handle))
            {
                return;
            }

            _listeners.erase(handle);
        }

        inline std::size_t listenersCount() const { return _listeners.size(); }
        
    private:
        std::map<ListenerHandle, CallbackFn> _listeners;
        ListenerHandle _counter = 0;
    };
}
