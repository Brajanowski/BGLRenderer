#pragma once

#include <memory>

#include "Log.h"
#include "OpenGLRenderer.h"

namespace BGLRenderer
{
    class Engine;

    class Application
    {
    public:
        Application() = default;
        virtual ~Application() = default;

        inline void setEngine(Engine* engine) { _engine = engine; }
        
        virtual void onInit() = 0;
        virtual void onShutdown() = 0;
        virtual void onUpdate(float deltaTime) = 0;
        virtual void onRender(const std::shared_ptr<OpenGLRenderer>& renderer) = 0;
        virtual void onIMGUI() = 0;
        virtual void onWindowResize(int width, int height) = 0;

    protected:
        Log _logger{"App"};

        Engine* _engine;
    };
}
