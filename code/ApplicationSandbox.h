﻿#pragma once

#include "Application.h"
#include "Config.h"
#include "PerspectiveCamera.h"

namespace BGLRenderer
{
    class ApplicationSandbox : public Application
    {
    public:
        ApplicationSandbox();
        
    protected:
        void onInit() override;
        void onShutdown() override;
        void onUpdate(float deltaTime) override;
        void onRender(const std::shared_ptr<OpenGLRenderer>& renderer) override;
        void onIMGUI() override;
        void onWindowResize(int width, int height) override;

    private:
        std::shared_ptr<Config> _sandboxConfig;

        std::shared_ptr<SceneObject> _monkey;
        std::shared_ptr<Scene> _scene;

        std::shared_ptr<PerspectiveCamera> _camera;
        float _cameraPitch = 0.0f;
        float _cameraYaw = 0.0f;

        float _cameraSpeed = 20.0f;

        void loadSandboxConfig();
    };
}
