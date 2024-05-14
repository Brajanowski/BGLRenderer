﻿#include "Application.h"

#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>
#include <functional>

namespace BGLRenderer
{
    Application::Application(int argc, char** argv)
    {
    }

    Application::~Application() = default;

    int Application::run()
    {
        _window = std::make_shared<SDLWindow>(1920, 1080);
        _window->setOnSDLEventCallback([](const SDL_Event *ev) {
            ImGui_ImplSDL2_ProcessEvent(ev);
        });

        _renderer = std::make_shared<OpenGLRenderer>(1920, 1080);
        _window->setOnWindowResizedCallback([&](int width, int height)
        {
            _renderer->resizeFrame(width, height);
        });

        _assetContentLoader = std::make_shared<AssetContentLoader>();

        initImgui();

        onInit();

        while (!_window->exitRequested())
        {
            _window->processEvents();

            _renderer->beginFrame();
            onUpdate();

            _renderer->render();
            
            tickImgui();

            _window->swapBuffers();
        }

        onShutdown();

        shutdownImgui();

        _renderer.reset();
        _window.reset();
        return 0;
    }

    void Application::initImgui()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui_ImplSDL2_InitForOpenGL(_window->sdlWindowHandle(), _window->glContext());
        ImGui_ImplOpenGL3_Init();
    }

    void Application::shutdownImgui()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
    }

    void Application::tickImgui()
    {
        ImGui_ImplSDL2_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(static_cast<float>(_window->width()), static_cast<float>(_window->height()));

        ImGui::NewFrame();

        onIMGUI();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}
