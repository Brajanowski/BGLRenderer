#include "ApplicationSandbox.h"

#include <imgui.h>

#include "Foundation/Engine.h"
#include "Foundation/GLMMath.h"

namespace BGLRenderer
{
    namespace ConfigKeys
    {
        static constexpr const char* SandboxConfigName = "sandbox.json";

        static constexpr const char* StartScene = "start_scene";
        static constexpr const char* WindowSize = "window_size";
        static constexpr const char* WindowVSync = "window_vsync";
    }

    ApplicationSandbox::ApplicationSandbox()
    {
    }

    void ApplicationSandbox::onInit()
    {
        _camera = std::make_shared<PerspectiveCamera>();
        _camera->transform.position = {4.317f, 6.5f, 17.0f};

        loadSandboxConfig();

        if (_scene == nullptr)
        {
            _scene = std::make_shared<Scene>("Empty scene");
        }

        auto pbrProgram = _engine->assets()->getProgram("shaders/gbuffer_default");

        std::shared_ptr<OpenGLRenderObject> sphereRenderObject = _engine->assets()->getModel("sphere_hres.gltf", pbrProgram);

#if 1
        auto pbrMaterial = std::make_shared<OpenGLMaterial>("PBR", MaterialType::opaque, MaterialTag::pbr, pbrProgram);
        pbrMaterial->setTexture2D("baseColor", _engine->assets()->getTexture2D("white"));
        pbrMaterial->setVector4("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1));
        pbrMaterial->setFloat("roughness", 0.0f);
        pbrMaterial->setFloat("metallic", 0.0f);

        constexpr int numX = 10;
        constexpr int numY = 10;

        for (int x = 0; x < numX; ++x)
        {
            for (int y = 0; y < numY; ++y)
            {
                std::shared_ptr<SceneObject> sphere = _scene->createSceneObject(std::format("Sphere {}x{}", x, y));
                sphere->transform().position = {x * 1.5f + 10.0f, y * 1.5f, 2.0f};
                sphere->transform().scale = {0.5, 0.5, 0.5};
                sphere->setSubmeshes(sphereRenderObject->submeshes());

                std::shared_ptr<OpenGLMaterial> sphereMaterial = std::make_shared<OpenGLMaterial>(*pbrMaterial);
                sphereMaterial->setFloat("roughness", static_cast<float>(x) / static_cast<float>(numX));
                sphereMaterial->setFloat("metallic", static_cast<float>(y) / static_cast<float>(numY));

                sphere->submeshes()[0].material = sphereMaterial;
            }
        }
#endif

        std::shared_ptr<OpenGLMaterial> monkeyMaterial = _engine->assets()->getMaterial("basic_blue.json");
        std::shared_ptr<OpenGLRenderObject> monkeyRenderObject = _engine->assets()->getModel("monkey.gltf", pbrProgram);

        _monkey = _scene->createSceneObject("Monkey");
        _monkey->transform().position = {0, 5, 0};

        std::vector<RenderObjectSubmesh> monkeySubmeshes = monkeyRenderObject->submeshes();
        monkeySubmeshes[0].material = monkeyMaterial;
        _monkey->setSubmeshes(monkeySubmeshes);

        // Test sphere
        std::shared_ptr<OpenGLMaterial> sphereMaterial = _engine->assets()->getMaterial("materials/M_Planks037B.json");

        _testSphere = _scene->createSceneObject("Test Sphere");
        _testSphere->transform().position = {0, 0, 0};

        std::vector<RenderObjectSubmesh> sphereSubmeshes = sphereRenderObject->submeshes();
        sphereSubmeshes[0].material = sphereMaterial;
        _testSphere->setSubmeshes(sphereSubmeshes);

        // quad
        _quad = _scene->createSceneObject("Quad");
        _quad->transform().position = {4, 4, 0};

        std::vector<RenderObjectSubmesh> quadSubmeshes;
        RenderObjectSubmesh quadSubmesh{};
        quadSubmesh.material = _engine->assets()->getMaterial("materials/M_Planks037B.json");
        quadSubmesh.mesh = _engine->renderer()->quadMesh();
        quadSubmeshes.push_back(quadSubmesh);
        _quad->setSubmeshes(quadSubmeshes);

        // Environment
        std::shared_ptr<OpenGLTexture2D> skyboxTexture = _engine->assets()->getTexture2DHDR("metro_noord_4k.hdr");
        skyboxTexture->setWrapMode(WrapMode::repeat);
        skyboxTexture->setFilterMode(FilterMode::linear);

        auto environmentMap = std::make_shared<OpenGLEnvironmentMap>("Environment Map");
        _engine->renderer()->generateEnvironmentMap(environmentMap, skyboxTexture);
        _engine->renderer()->setEnvironmentMap(environmentMap);
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

            _camera->transform.rotation = glm::quat(glm::vec3(glm::radians(_cameraPitch), glm::radians(_cameraYaw),
                                                              0.0f));
        }

        float t = glm::sin(static_cast<float>(_engine->secondsSinceStart()) * 0.05f);
        _monkey->transform().rotation = glm::rotate(glm::mat4(1.0f), t * glm::pi<float>() * 2.0f, glm::vec3(0, 1, 0));
        _quad->transform().rotation = glm::rotate(glm::mat4(1.0f), _quadRot, glm::vec3(0, 1, 0));
        _testSphere->transform().rotation = glm::rotate(glm::mat4(1.0f), t * glm::pi<float>() * 2.0f,
                                                        glm::vec3(0, 1, 0));
    }

    void ApplicationSandbox::onRender(const std::shared_ptr<OpenGLRenderer>& renderer)
    {
        renderer->setCamera(_camera);

        for (const auto& sceneObject: _scene->objects())
        {
            glm::mat4x4 model = sceneObject->transform().modelMatrix();

            for (const auto& submesh: sceneObject->submeshes())
            {
                renderer->submit(submesh.material, submesh.mesh, model);
            }
        }

        renderer->gizmos().coordinateSystem(_monkey->transform().position, _monkey->transform().modelMatrix());
        renderer->gizmos().wireCube(_monkey->transform().position, {1, 1, 1});
    }

    void ApplicationSandbox::onIMGUI()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Windows"))
            {
                if (ImGui::MenuItem("Console"))
                {
                    _engine->setConsoleVisibility(true);
                }

                if (ImGui::MenuItem("Stats"))
                {
                    _engine->setStatsWindowVisibility(true);
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        ImGui::Begin("TEMP");
        ImGui::SliderAngle("Quad Rot", &_quadRot);
        ImGui::End();

        ImGui::Begin("Camera");

        ImGui::SliderFloat("Speed", &_cameraSpeed, 1.0f, 50.0f);

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

    void ApplicationSandbox::loadSandboxConfig()
    {
        _sandboxConfig = _engine->assets()->getConfig(ConfigKeys::SandboxConfigName);

        if (_sandboxConfig->exists(ConfigKeys::WindowSize))
        {
            glm::vec2 size = _sandboxConfig->getVector2(ConfigKeys::WindowSize);

            int newWidth = static_cast<int>(size.x);
            int newHeight = static_cast<int>(size.y);

            if (newWidth > 0 && newHeight > 0)
            {
                _engine->window()->resize(newWidth, newHeight);
            }
            else
            {
                _logger.error("Invalid window size in sandbox config file");
            }
        }

        if (_sandboxConfig->exists(ConfigKeys::WindowVSync))
        {
            int vsync = _sandboxConfig->getInt(ConfigKeys::WindowVSync);
            _engine->window()->setVSync(vsync);
        }

        if (_sandboxConfig->exists(ConfigKeys::StartScene))
        {
            _scene = _engine->assets()->getScene(_sandboxConfig->getString(ConfigKeys::StartScene));
        }
    }
}
