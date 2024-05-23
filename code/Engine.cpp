#include "Engine.h"

#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>
#include <functional>

namespace BGLRenderer
{
    Engine::Engine(int argc, char** argv)
    {
    }

    Engine::~Engine()
    {
    }

    int Engine::run()
    {
        Log::listen([](const std::string& message)
        {
            std::cout << message << std::endl;
        });

        _consoleWindow = std::make_shared<ConsoleWindow>();

        _input = std::make_shared<Input>();
        _assetContentLoader = std::make_shared<AssetContentLoader>();
        _assetManager = std::make_shared<AssetManager>(_assetContentLoader);

        _window = std::make_shared<SDLWindow>(1920, 1080);
        _window->setOnSDLEventCallback([&](const SDL_Event* ev)
        {
            ImGui_ImplSDL2_ProcessEvent(ev);
            _input->processSDLEvent(ev);
        });

        _renderer = std::make_shared<OpenGLRenderer>(_assetManager, 1920, 1080);
        _window->setOnWindowResizedCallback([&](int width, int height)
        {
            _renderer->resizeFrame(width, height);
            _application->onWindowResize(width, height);
        });

        _window->setVSync(false);

        initImgui();

        _application->onInit();

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

            _application->onUpdate(static_cast<float>(deltaTime));
            _assetManager->tick();

            renderTimer.restart();
            _renderer->beginFrame();
            _application->onRender(_renderer);
            _renderer->endFrame();

            _profilerData.renderTime = renderTimer.elapsedMilliseconds();

            imguiTimer.restart();
            tickImgui();
            _profilerData.imguiTime = renderTimer.elapsedMilliseconds();

            _window->swapBuffers();

            _profilerData.totalFrameTime = frameTimer.elapsedMilliseconds();
        }

        _application->onShutdown();
        _application.reset(nullptr);

        shutdownImgui();

        _assetManager.reset();
        _assetContentLoader.reset();
        _renderer.reset();
        _input.reset();
        _window.reset();

        return 0;
    }

    void Engine::profilerWindow()
    {
        ImGui::Begin("App profiler");

        ImGui::Text("FPS: %d", _profilerData.fps);
        ImGui::Text("Frame: %.4fms", _profilerData.totalFrameTime);
        ImGui::Text("Render: %.4fms", _profilerData.renderTime);
        ImGui::Text("ImGui: %.4fms", _profilerData.imguiTime);

        ImGui::End();
    }

    double Engine::secondsSinceStart()
    {
        return _appTimer.elapsedSeconds();
    }

    void Engine::initImgui()
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

    void Engine::shutdownImgui()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
    }

    void Engine::tickImgui()
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

    void Engine::onIMGUI()
    {
        _application->onIMGUI();
        _renderer->onIMGUI();

        if (_showProfilerWindow)
        {
            profilerWindow();
        }

        if (_showConsoleWindow)
        {
            _consoleWindow->onIMGUI();
        }
    }
}
