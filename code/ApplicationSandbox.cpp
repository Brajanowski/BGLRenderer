#include "ApplicationSandbox.h"

#include <imgui.h>
#include "GLMMath.h"

namespace BGLRenderer
{
    ApplicationSandbox::ApplicationSandbox(int argc, char** argv) : Application(argc, argv)
    {
    }

    void ApplicationSandbox::onInit()
    {
        _basicMaterial = std::make_shared<OpenGLMaterial>(_assetsLoader->loadProgram("shaders/basic"));

        std::shared_ptr<OpenGLTexture2D> avatarTexture = _assetsLoader->loadTexture("avatar.png");
        _basicMaterial->setTexture2D("textureTest", avatarTexture);

        _quadMesh = std::make_shared<OpenGLMesh>();

        GLfloat vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.5f, 0.5f, 0.0f,
            -0.5f, 0.5f, 0.0f,
        };

        GLfloat normals[] = {
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f
        };

        GLfloat uvs[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };

        GLuint indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        _quadMesh->setVertices(vertices, sizeof(vertices) / sizeof(GLfloat));
        _quadMesh->setNormals(normals, sizeof(normals) / sizeof(GLfloat));
        _quadMesh->setUVs0(uvs, sizeof(uvs) / sizeof(GLfloat));
        _quadMesh->setIndices(indices, sizeof(indices) / sizeof(GLuint));

        _camera = std::make_shared<PerspectiveCamera>();
        _camera->transform.position = {0.1f, 0, 5.0f};
    }

    void ApplicationSandbox::onShutdown()
    {
    }

    void ApplicationSandbox::onUpdate(float deltaTime)
    {
        constexpr float cameraSpeed = 5.0f;
        constexpr float cameraSensitivity = 0.7f;

        if (_input->keyboard()->getKey(SDLK_w))
        {
            _camera->transform.position += _camera->forward() * cameraSpeed * deltaTime;
        }

        if (_input->keyboard()->getKey(SDLK_s))
        {
            _camera->transform.position += _camera->forward() * cameraSpeed * deltaTime * -1.0f;
        }

        if (_input->keyboard()->getKey(SDLK_d))
        {
            _camera->transform.position += _camera->right() * cameraSpeed * deltaTime;
        }

        if (_input->keyboard()->getKey(SDLK_a))
        {
            _camera->transform.position += _camera->right() * cameraSpeed * deltaTime * -1.0f;
        }

        if (_input->keyboard()->getKey(SDLK_e))
        {
            _camera->transform.position += _camera->up() * cameraSpeed * deltaTime;
        }

        if (_input->keyboard()->getKey(SDLK_q))
        {
            _camera->transform.position += _camera->up() * cameraSpeed * deltaTime * -1.0f;
        }

        if (_input->mouse()->getButton(SDL_BUTTON_RIGHT))
        {
            glm::vec2 deltaPosition = _input->mouse()->deltaPosition();

            _cameraPitch -= deltaPosition.y * cameraSensitivity;
            _cameraYaw -= deltaPosition.x * cameraSensitivity;

            _camera->transform.rotation = glm::quat(glm::vec3(glm::radians(_cameraPitch), glm::radians(_cameraYaw), 0.0f));
        }

        _basicMaterial->setFloat("test", glm::sin((float)secondsSinceStart()));

        glm::mat4x4 viewProjection = _camera->viewProjection();
        _basicMaterial->setMatrix4x4("viewProjection", viewProjection);
    }

    void ApplicationSandbox::onRender()
    {
        //GL_CALL(glEnable(GL_CULL_FACE));
        //GL_CALL(glCullFace(GL_FRONT));
        _basicMaterial->bind();

        _quadMesh->bind();
        _quadMesh->draw();
    }

    void ApplicationSandbox::onIMGUI()
    {
        profilerWindow();

        ImGui::Begin("Camera");

        ImGui::InputFloat3("Position", glm::value_ptr(_camera->transform.position));
        ImGui::InputFloat("Pitch", &_cameraPitch);
        ImGui::InputFloat("Yaw", &_cameraYaw);

        _camera->transform.rotation = glm::quat(glm::vec3(glm::radians(_cameraPitch), glm::radians(_cameraYaw), 0.0f));

        glm::vec3 forward = _camera->forward();
        glm::vec3 right = _camera->right();
        glm::vec3 up = _camera->up();
        
        ImGui::Text("Forward: %.4f, %.4f, %.4f", forward.x, forward.y, forward.z);
        ImGui::Text("Right: %.4f, %.4f, %.4f", right.x, right.y, right.z);
        ImGui::Text("Up: %.4f, %.4f, %.4f", up.x, up.y, up.z);

        if (ImGui::Button("Print matrices"))
        {
            printMatrices();
        }

        ImGui::End();
    }

    void ApplicationSandbox::onWindowResize(int width, int height)
    {
        _camera->aspectRatio = (float)width / (float)height;
    }

    void ApplicationSandbox::printMatrices()
    {
        printVec3(_camera->transform.position);
        printMatrix("View matrix", _camera->view());
        printMatrix("Projection matrix", _camera->projection());
    }
}
