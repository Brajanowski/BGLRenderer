﻿#include "ApplicationSandbox.h"

#include <imgui.h>

#include "Engine.h"
#include "GLMMath.h"

namespace BGLRenderer
{
    namespace Debug
    {
        constexpr bool LoadSponza = true;
    }
    
    ApplicationSandbox::ApplicationSandbox()
    {
    }

    void ApplicationSandbox::onInit()
    {
        _camera = std::make_shared<PerspectiveCamera>();
        _camera->transform.position = {0.0f, 4.963f, 5.0f};

        auto monkeyMaterial = _engine->assets()->getMaterial("basicRed.json");
        _monkey = _engine->assets()->getModel("monkey.gltf", _engine->assets()->getProgram("shaders/gbuffer_default"));
        _monkey->submeshes()[0].material = monkeyMaterial;

        if constexpr (Debug::LoadSponza)
        {
            _sponza = _engine->assets()->getModel("sponza/sponza.gltf", _engine->assets()->getProgram("shaders/gbuffer_default"));

            const float sponzaScaleFactor = 0.1f;
            glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(sponzaScaleFactor, sponzaScaleFactor, sponzaScaleFactor));

            _sponza->setModelMatrix(scaleMatrix);
        }
    }

    void ApplicationSandbox::onShutdown()
    {
    }

    void ApplicationSandbox::onUpdate(float deltaTime)
    {
        constexpr float cameraSensitivity = 0.7f;

        std::shared_ptr<Input> input = _engine->input();

        if (input->keyboard()->getKey(SDLK_w))
        {
            _camera->transform.position += _camera->forward() * _cameraSpeed * deltaTime;
        }

        if (input->keyboard()->getKey(SDLK_s))
        {
            _camera->transform.position += _camera->forward() * _cameraSpeed * deltaTime * -1.0f;
        }

        if (input->keyboard()->getKey(SDLK_d))
        {
            _camera->transform.position += _camera->right() * _cameraSpeed * deltaTime;
        }

        if (input->keyboard()->getKey(SDLK_a))
        {
            _camera->transform.position += _camera->right() * _cameraSpeed * deltaTime * -1.0f;
        }

        if (input->keyboard()->getKey(SDLK_e))
        {
            _camera->transform.position += _camera->up() * _cameraSpeed * deltaTime;
        }

        if (input->keyboard()->getKey(SDLK_q))
        {
            _camera->transform.position += _camera->up() * _cameraSpeed * deltaTime * -1.0f;
        }

        if (input->mouse()->getButton(SDL_BUTTON_RIGHT))
        {
            glm::vec2 deltaPosition = input->mouse()->deltaPosition();

            _cameraPitch -= deltaPosition.y * cameraSensitivity;
            _cameraYaw -= deltaPosition.x * cameraSensitivity;

            _camera->transform.rotation = glm::quat(glm::vec3(glm::radians(_cameraPitch), glm::radians(_cameraYaw), 0.0f));
        }

        float t = glm::sin(static_cast<float>(_engine->secondsSinceStart()) * 0.05f);
        glm::mat4 monkeyTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(0, 5, 0));
        glm::mat4 monkeyRotation = glm::rotate(glm::mat4(1.0f), t * glm::pi<float>() * 2.0f, glm::vec3(0, 1, 0));

        _monkey->setModelMatrix(monkeyRotation * monkeyTranslation);
    }

    void ApplicationSandbox::onRender(const std::shared_ptr<OpenGLRenderer>& renderer)
    {
        renderer->setCamera(_camera);

        renderer->submit(_monkey);

        if constexpr (Debug::LoadSponza)
        {
            renderer->submit(_sponza);
        }
    }

    void ApplicationSandbox::onIMGUI()
    {
        ImGui::Begin("Camera");

        ImGui::InputFloat("Speed", &_cameraSpeed);

        ImGui::InputFloat("FOV", &_camera->fieldOfView);
        ImGui::InputFloat("Near Z", &_camera->nearZ);
        ImGui::InputFloat("Far Z", &_camera->farZ);
        
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

        ImGui::End();
    }

    void ApplicationSandbox::onWindowResize(int width, int height)
    {
        _camera->aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    }
}
