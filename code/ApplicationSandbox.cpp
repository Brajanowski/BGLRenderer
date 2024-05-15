#include "ApplicationSandbox.h"

#include <imgui.h>

namespace BGLRenderer
{
    ApplicationSandbox::ApplicationSandbox(int argc, char** argv) : Application(argc, argv)
    {
    }

    void ApplicationSandbox::onInit()
    {
        std::vector<std::uint8_t> basicVertContent = _assetContentLoader->load("shaders/basic.vert");
        std::vector<std::uint8_t> basicFragmentContent = _assetContentLoader->load("shaders/basic.frag");

        _basicProgram = std::make_shared<OpenGLProgram>(std::string(basicVertContent.begin(), basicVertContent.end()), std::string(basicFragmentContent.begin(), basicFragmentContent.end()));
        _basicMaterial = std::make_shared<OpenGLMaterial>(_basicProgram);

        _quadMesh = std::make_shared<OpenGLMesh>();

        GLfloat vertices[] = {
            0.0f, 0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f
        };

        GLuint indices[] = {
            0, 1, 2
        };
        
        _quadMesh->setVertices(vertices, sizeof(vertices) / sizeof(GLfloat));
        _quadMesh->setIndices(indices, sizeof(indices) / sizeof(GLuint));
    }

    void ApplicationSandbox::onShutdown()
    {
    }

    void ApplicationSandbox::onRender()
    {
        _basicMaterial->bind();

        _quadMesh->bind();
        _quadMesh->draw();
        
    }

    void ApplicationSandbox::onIMGUI()
    {
        ImGui::Begin("TEST");
        ImGui::Text("TEST");
        ImGui::End();

        profilerWindow();
    }
}
