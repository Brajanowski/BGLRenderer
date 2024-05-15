﻿#pragma once

#include <memory>

#include "OpenGLRenderer.h"
#include "SDLWindow.h"
#include "AssetContentLoader.h"

namespace BGLRenderer
{
    struct ApplicationProfilerData
    {
        double totalFrameTime;
        double renderTime;
        double imguiTime;
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
        virtual void onRender() = 0;
        virtual void onIMGUI() = 0;

    private:
        void initImgui();
        void shutdownImgui();
        void tickImgui();
    };
}