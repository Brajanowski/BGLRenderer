#pragma once

#include "Application.h"

#include "OpenGLRenderer.h"
#include "SDLWindow.h"
#include "AssetContentLoader.h"
#include "AssetManager.h"
#include "ConsoleWindow.h"
#include "Input.h"
#include "Log.h"
#include "Timer.h"

namespace BGLRenderer
{
    struct EngineProfilerData
    {
        double totalFrameTime = 0.0;
        double renderTime = 0.0;
        double imguiTime = 0.0;
        int fps = 0;
    };

    class Engine
    {
    public:
        Engine(int argc, char **argv);
        ~Engine();

        int run();

        template<class T>
        void setApplication()
        {
            _application = std::make_unique<T>();
            _application->setEngine(this);
        }

        double secondsSinceStart();

        inline const std::shared_ptr<Input>& input() const { return _input; }
        inline const std::shared_ptr<AssetManager>& assets() const { return _assetManager; }

        inline void setStatsWindowVisibility(bool visible) { _showStatsWindow = visible; }
        inline bool isStatsWindowVisible() const { return _showStatsWindow; }

        inline bool isConsoleVisible() const { return _showConsoleWindow; }
        inline void setConsoleVisibility(bool visible) { _showConsoleWindow = visible; }
        
    private:
        Log _logger{"Engine"};

        std::shared_ptr<SDLWindow> _window = nullptr;
        std::shared_ptr<OpenGLRenderer> _renderer = nullptr;
        std::shared_ptr<AssetContentLoader> _assetContentLoader = nullptr;
        std::shared_ptr<AssetManager> _assetManager = nullptr;
        std::shared_ptr<Input> _input = nullptr;
        std::shared_ptr<ConsoleWindow> _consoleWindow = nullptr;

        std::unique_ptr<Application> _application = nullptr;

        EngineProfilerData _profilerData;

        bool _showStatsWindow = true;
        bool _showConsoleWindow = true;

        void statsWindow();

        void initImgui();
        void shutdownImgui();
        void tickImgui();

        void onIMGUI();

        HighResolutionTimer _appTimer;
    };
}
