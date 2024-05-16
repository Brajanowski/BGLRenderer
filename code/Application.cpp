#include "Application.h"

#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>
#include <functional>

#include "Timer.h" 

namespace BGLRenderer
{
    Application::Application(int argc, char** argv)
    {
    }

    Application::~Application() = default;

    int Application::run()
    {
        _input = std::make_shared<Input>();
        
        _window = std::make_shared<SDLWindow>(1920, 1080);
        _window->setOnSDLEventCallback([&](const SDL_Event *ev) {
            ImGui_ImplSDL2_ProcessEvent(ev);
            _input->processSDLEvent(ev);
        });

        _renderer = std::make_shared<OpenGLRenderer>(1920, 1080);
        _window->setOnWindowResizedCallback([&](int width, int height)
        {
            _renderer->resizeFrame(width, height);
            onWindowResize(width, height);
        });

        _window->setVSync(false);

        _assetContentLoader = std::make_shared<AssetContentLoader>();
        _assetsLoader = std::make_shared<AssetsLoader>(_assetContentLoader);

        initImgui();

        onInit();

        HighResolutionTimer frameTimer;
        HighResolutionTimer renderTimer;
        HighResolutionTimer imguiTimer;

        double fpsTimer = 0.0;
        int fpsCounter = 0;

        _appTimer.restart();
        while (!_window->exitRequested())
        {
            double deltaTime = frameTimer.elapsedSeconds();

            fpsTimer += deltaTime;
            fpsCounter++;

            if (fpsTimer >= 1.0)
            {
                _profilerData.fps = fpsCounter;
                fpsCounter = 0;
                fpsTimer = 0.0;
            }
            
            frameTimer.restart();

            _input->startFrame();
            _window->processEvents();

            onUpdate(static_cast<float>(deltaTime));

            renderTimer.restart();
            _renderer->beginFrame();
            onRender();
            _renderer->render();

            _profilerData.renderTime = renderTimer.elapsedMilliseconds();

            imguiTimer.restart();
            tickImgui();
            _profilerData.imguiTime = renderTimer.elapsedMilliseconds();

            _window->swapBuffers();
            
            _profilerData.totalFrameTime = frameTimer.elapsedMilliseconds();
        }

        onShutdown();

        shutdownImgui();

        _renderer.reset();
        _window.reset();
        return 0;
    }

    void Application::profilerWindow()
    {
        ImGui::Begin("App profiler");

        ImGui::Text("FPS: %d", _profilerData.fps);
        ImGui::Text("Frame: %.4fms", _profilerData.totalFrameTime);
        ImGui::Text("Render: %.4fms", _profilerData.renderTime);
        ImGui::Text("ImGui: %.4fms", _profilerData.imguiTime);

        ImGui::End();
    }

    double Application::secondsSinceStart()
    {
        return _appTimer.elapsedSeconds();
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
