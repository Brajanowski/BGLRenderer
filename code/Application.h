#pragma once

#include <memory>

#include "OpenGLRenderer.h"
#include "SDLWindow.h"
#include "AssetContentLoader.h"
#include "Timer.h"

namespace BGLRenderer
{
    struct ApplicationProfilerData
    {
        double totalFrameTime = 0.0;
        double renderTime = 0.0;
        double imguiTime = 0.0;
        int fps = 0;
    };
    
    class Application
    {
    public:
        Application(int argc, char **argv);
        virtual ~Application();

        int run();

    protected:
        std::shared_ptr<SDLWindow> _window = nullptr;
        std::shared_ptr<OpenGLRenderer> _renderer = nullptr;
        std::shared_ptr<AssetContentLoader> _assetContentLoader = nullptr;

        ApplicationProfilerData _profilerData;

        void profilerWindow();
        
        virtual void onInit() = 0;
        virtual void onShutdown() = 0;
        virtual void onUpdate(float deltaTime) = 0;
        virtual void onRender() = 0;
        virtual void onIMGUI() = 0;
        virtual void onWindowResize(int width, int height) = 0;

        double secondsSinceStart();

    private:
        void initImgui();
        void shutdownImgui();
        void tickImgui();

        HighResolutionTimer _appTimer;
    };
}
