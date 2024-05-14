#pragma once

#include "Application.h"
#include "OpenGLMesh.h"

namespace BGLRenderer
{
    class ApplicationSandbox : public Application
    {
    public:
        ApplicationSandbox(int argc, char** argv);
        
    protected:
        virtual void onInit() override;
        virtual void onShutdown() override;
        virtual void onUpdate() override;
        virtual void onIMGUI() override;

    private:
        std::shared_ptr<OpenGLProgram> _basicProgram;
        std::shared_ptr<OpenGLMaterial> _basicMaterial;
        std::shared_ptr<OpenGLMesh> _quadMesh;
    };
}
