#pragma once

#include <SDL.h>

#include "GLMMath.h"
#include "Log.h"

namespace BGLRenderer
{
    constexpr unsigned int MaxKeys = 1024;
    constexpr unsigned int MaxButtons = 512;

    class Mouse
    {
    public:
        inline bool getButton(unsigned int button) const { return _buttons[button]; }
        inline bool getButtonDown(unsigned int button) const { return _buttonsDown[button]; }
        inline bool getButtonUp(unsigned int button) const { return _buttonsUp[button]; }

        inline glm::vec2 position() const { return _mousePosition; }
        inline glm::vec2 deltaPosition() const { return _deltaPosition; }

        void startFrame();
        void processSDLEvent(const SDL_Event* ev);

    private:
        static Log _logger;

        bool _buttons[MaxButtons] = {};
        bool _buttonsDown[MaxButtons] = {};
        bool _buttonsUp[MaxButtons] = {};
        
        glm::vec2 _mousePosition = {0.0f, 0.0f};
        glm::vec2 _deltaPosition = {0.0f, 0.0f};
    };

    class Keyboard
    {
    public:
        inline bool getKey(unsigned int keycode) const { return _keys[keycode]; }
        inline bool getKeyDown(unsigned int keycode) const { return _keysDown[keycode]; }
        inline bool getKeyUp(unsigned int keycode) const { return _keysUp[keycode]; }

        void startFrame();
        void processSDLEvent(const SDL_Event* ev);

    private:
        static Log _logger;

        bool _keys[MaxKeys] = {};
        bool _keysDown[MaxKeys] = {};
        bool _keysUp[MaxKeys] = {};
    };

    class Input
    {
    public:
        Input() = default;

        void startFrame();
        void processSDLEvent(const SDL_Event* ev);

        inline const Mouse* mouse() const { return &_mouse; }
        inline const Keyboard* keyboard() const { return &_keyboard; }

    private:
        static Log _logger;
        
        Mouse _mouse;
        Keyboard _keyboard;
    };
}
