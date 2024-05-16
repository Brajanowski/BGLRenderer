#include "Input.h"

#include <iostream>

namespace BGLRenderer
{
    void Mouse::startFrame()
    {
        std::memset(_buttonsDown, false, MaxButtons * sizeof(bool));
        std::memset(_buttonsUp, false, MaxButtons * sizeof(bool));
        _deltaPosition = {0.0f, 0.0f};
    }

    void Mouse::processSDLEvent(const SDL_Event* ev)
    {
        if (ev->type == SDL_MOUSEMOTION)
        {
            _mousePosition = {static_cast<float>(ev->motion.x), static_cast<float>(ev->motion.y)};
            _deltaPosition = {static_cast<float>(ev->motion.xrel), static_cast<float>(ev->motion.yrel)};
        }
        else if (ev->type == SDL_MOUSEBUTTONDOWN)
        {
            unsigned int button = ev->button.button;
            _buttons[button] = true;
            _buttonsDown[button] = true;
        }
        else if (ev->type == SDL_MOUSEBUTTONUP)
        {
            unsigned int button = ev->button.button;
            _buttons[button] = false;
            _buttonsUp[button] = true;
        }
    }

    void Keyboard::startFrame()
    {
        std::memset(_keysDown, false, MaxKeys * sizeof(bool));
        std::memset(_keysUp, false, MaxKeys * sizeof(bool));
    }

    void Keyboard::processSDLEvent(const SDL_Event* ev)
    {
        if (ev->type == SDL_KEYDOWN)
        {
            unsigned int keycode = ev->key.keysym.sym;
            if (keycode >= MaxKeys)
            {
                std::printf("Keycode is not supported!");
                return;
            }

            _keys[keycode] = true;
            _keysDown[keycode] = true;
        }
        else if (ev->type == SDL_KEYDOWN)
        {
            unsigned int keycode = ev->key.keysym.sym;
            if (keycode >= MaxKeys)
            {
                std::printf("Keycode is not supported!");
                return;
            }

            _keys[keycode] = false;
            _keysUp[keycode] = true;
        }
    }

    void Input::startFrame()
    {
        _mouse.startFrame();
        _keyboard.startFrame();
    }

    void Input::processSDLEvent(const SDL_Event* ev)
    {
        _mouse.processSDLEvent(ev);
        _keyboard.processSDLEvent(ev);
    }
}
