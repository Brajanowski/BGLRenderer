#pragma once

#include "Application.h"
#include "PerspectiveCamera.h"

namespace BGLRenderer
{
    class ApplicationSandbox : public Application
    {
    public:
        ApplicationSandbox();
        
    protected:
        virtual void onInit() override;
        virtual void onShutdown() override;
        virtual void onUpdate(float deltaTime) override;
        virtual void onRender(const std::shared_ptr<OpenGLRenderer>& renderer) override;
        virtual void onIMGUI() override;
        virtual void onWindowResize(int width, int height) override;

    private:
        std::shared_ptr<SceneObject> _monkey;
        std::shared_ptr<Scene> _scene;

        std::shared_ptr<PerspectiveCamera> _camera;
        float _cameraPitch = 0.0f;
        float _cameraYaw = 0.0f;

        float _cameraSpeed = 20.0f;
    };
}
