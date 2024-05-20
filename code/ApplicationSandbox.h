#pragma once

#include "Application.h"
#include "OpenGLMesh.h"
#include "PerspectiveCamera.h"

namespace BGLRenderer
{
    class ApplicationSandbox : public Application
    {
    public:
        ApplicationSandbox(int argc, char** argv);
        
    protected:
        virtual void onInit() override;
        virtual void onShutdown() override;
        virtual void onUpdate(float deltaTime) override;
        virtual void onRender(const std::shared_ptr<OpenGLRenderer>& renderer) override;
        virtual void onIMGUI() override;
        virtual void onWindowResize(int width, int height) override;

    private:
        std::shared_ptr<OpenGLMaterial> _basicMaterial;
        std::shared_ptr<OpenGLMesh> _quadMesh;

        std::shared_ptr<OpenGLRenderObject> _monkey;
        std::shared_ptr<OpenGLRenderObject> _sponza;

        std::shared_ptr<PerspectiveCamera> _camera;
        float _cameraPitch = 0.0f;
        float _cameraYaw = 0.0f;

        float _cameraSpeed = 20.0f;

        void printMatrices();
    };
}
