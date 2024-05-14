#pragma once

#include <SDL.h>
#include <functional>

namespace BGLRenderer
{
    typedef std::function<void(const SDL_Event *)> OnSdlEventCallbackFn;
    typedef std::function<void(int width, int height)> OnWindowResizedCallbackFn;

    class SDLWindow
    {
    public:
        SDLWindow(int width, int height);
        ~SDLWindow();

        void processEvents();
        void swapBuffers();

        inline void setOnSDLEventCallback(OnSdlEventCallbackFn onSdlEventCallback) { _onSDLEventCallback = onSdlEventCallback; }
        inline void setOnWindowResizedCallback(OnWindowResizedCallbackFn onWindowResizedCallback) { _onWindowResizedCallback = onWindowResizedCallback; }

        inline bool exitRequested() const { return _exitRequested; }
        inline int width() const { return _width; }
        inline int height() const { return _height; }

        inline SDL_Window *sdlWindowHandle() const { return _sdlWindow; }
        inline SDL_GLContext glContext() const { return _glContext; }

    private:
        int _width = 0;
        int _height = 0;

        SDL_Window *_sdlWindow = nullptr;
        SDL_GLContext _glContext = nullptr;

        OnSdlEventCallbackFn _onSDLEventCallback;
        OnWindowResizedCallbackFn _onWindowResizedCallback;

        bool _exitRequested = false;

        void createWindow();
        void destroyWindow();
    };
}
