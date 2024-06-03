#include "SDLWindow.h"

#include <glad/glad.h>

namespace BGLRenderer
{
    SDLWindow::SDLWindow(int width, int height) :
        _width(width),
        _height(height)
    {
        createWindow();
    }

    SDLWindow::~SDLWindow()
    {
        destroyWindow();
    }

    void SDLWindow::setVSync(bool enable)
    {
        SDL_GL_SetSwapInterval(enable ? 1 : 0);
    }

    void SDLWindow::resize(int width, int height)
    {
        SDL_SetWindowSize(_sdlWindow, width, height);

        _width = width;
        _height = height;

        if (_onWindowResizedCallback != nullptr)
        {
            _onWindowResizedCallback(_width, _height);
        }
    }

    void SDLWindow::processEvents()
    {
        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_QUIT)
            {
                _exitRequested = true;
            }
            else if (ev.type == SDL_WINDOWEVENT)
            {
                if (ev.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    _width = ev.window.data1;
                    _height = ev.window.data2;

                    if (_onWindowResizedCallback != nullptr)
                    {
                        _onWindowResizedCallback(_width, _height);
                    }
                }
            }

            if (_onSDLEventCallback != nullptr)
            {
                _onSDLEventCallback(&ev);
            }
        }
    }

    void SDLWindow::swapBuffers()
    {
        SDL_GL_SwapWindow(_sdlWindow);
    }

    void SDLWindow::createWindow()
    {
        SDL_Init(SDL_INIT_VIDEO);

        SDL_GL_LoadLibrary(nullptr);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        _sdlWindow = SDL_CreateWindow("BGLrenderer",
                                      SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      _width,
                                      _height,
                                      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

        _exitRequested = false;

        _glContext = SDL_GL_CreateContext(_sdlWindow);

        gladLoadGLLoader(SDL_GL_GetProcAddress);
    }

    void SDLWindow::destroyWindow()
    {
        SDL_GL_DeleteContext(_glContext);
        _glContext = nullptr;

        SDL_DestroyWindow(_sdlWindow);
        _sdlWindow = nullptr;

        SDL_Quit();
    }
}
