#include "OpenGLRenderer.h"

#include <imgui.h>

namespace BGLRenderer
{
    constexpr int GBufferAlbedoAttachment = 0;
    constexpr int GBufferNormalsAttachment = 1;

    OpenGLRenderer::OpenGLRenderer(const std::shared_ptr<AssetsLoader>& assetsLoader, int frameWidth, int frameHeight) :
        _assetsLoader(assetsLoader),
        _frameWidth(frameWidth),
        _frameHeight(frameHeight)
    {
        initializeDefaultResources();
    }

    OpenGLRenderer::~OpenGLRenderer()
    {
    }

    void OpenGLRenderer::onIMGUI()
    {
        ImGui::Begin("Renderer");

        ImGui::ColorEdit3("Ambient Light", glm::value_ptr(_ambientLight));
        ImGui::InputFloat3("Directional Light - Direction", glm::value_ptr(_directionalLightDirection));
        ImGui::InputFloat3("Directional Light - Color", glm::value_ptr(_directionalLightColor));
        ImGui::InputFloat("Directional Light - Intensity", &_directionalLightIntensity);

        static const char* bufferToDisplayStrings[] = {
            "Final Frame",
            "Albedo",
            "Normal",
            "Depth",
            "Light"
        };
        static BufferToDisplay bufferToDisplayValues[] = {
            BufferToDisplay::finalFrame,
            BufferToDisplay::albedo,
            BufferToDisplay::normal,
            BufferToDisplay::depth,
            BufferToDisplay::lightBuffer
        };
        static constexpr int bufferToDisplayCount = 5;

        int selectedItem = 0;
        for (int i = 0; i < bufferToDisplayCount; ++i)
        {
            if (_bufferToDisplay == bufferToDisplayValues[i])
            {
                selectedItem = i;
                break;
            }
        }

        ImGui::Combo("Buffer to display", &selectedItem, bufferToDisplayStrings, bufferToDisplayCount);
        _bufferToDisplay = bufferToDisplayValues[selectedItem];

        ImGui::End();
    }

    void OpenGLRenderer::resizeFrame(int width, int height)
    {
        _frameWidth = width;
        _frameHeight = height;

        _gbuffer->resize(width, height);
        _lightBuffer->resize(width, height);
    }

    void OpenGLRenderer::beginFrame()
    {
        _renderObjects.clear();
    }

    void OpenGLRenderer::endFrame()
    {
        _viewProjection = _camera->viewProjection();
        
        gbufferPass();
        lightPass();
        finalFramePass();
    }

    void OpenGLRenderer::initializeDefaultResources()
    {
        _logger.debug("Initializing default resources");

        _quadMesh = std::make_shared<OpenGLMesh>();

        GLfloat vertices[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
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

        _baseColorProgram = _assetsLoader->loadProgram("shaders/base_color");

        // GBuffer
        _gbuffer = std::make_shared<OpenGLFramebuffer>(_frameWidth, _frameHeight);
        // DO NOT CHANGE THE ORDER
        _gbuffer->createColorAttachment(GL_RGBA); // Albedo
        _gbuffer->createColorAttachment(GL_RGBA); // Normal map
        _gbuffer->createDepthAttachment(GL_DEPTH_COMPONENT);
        _gbuffer->validate();

        _lightBuffer = std::make_shared<OpenGLFramebuffer>(_frameWidth, _frameHeight);
        _lightBuffer->createColorAttachment(GL_RGB);
        _lightBuffer->validate();

        _directionalLightProgram = _assetsLoader->loadProgram("shaders/light_directional");
        _combineFinalFrameProgram = _assetsLoader->loadProgram("shaders/combine_finalframe");

        _displayBufferMaterial = std::make_shared<OpenGLMaterial>(_assetsLoader->loadProgram("shaders/basic"));
    }

    void OpenGLRenderer::gbufferPass()
    {
        _gbuffer->bind();

        GLenum drawTargets[] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1
        };
        GL_CALL(glDrawBuffers(2, drawTargets));

        glViewport(0, 0, _frameWidth, _frameHeight);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GL_CALL(glEnable(GL_DEPTH_TEST));
        GL_CALL(glDepthFunc(GL_LEQUAL));

        GL_CALL(glEnable(GL_CULL_FACE));
        GL_CALL(glCullFace(GL_BACK));

        for (const auto& renderObject : _renderObjects)
        {
            glm::mat4 model = renderObject->modelMatrix();

            for (const auto& submesh : renderObject->submeshes())
            {
                submesh.material->bind();
                
                // TODO: cache uniform locations
                submesh.material->program()->setMatrix4x4(submesh.material->program()->getUniformLocation("u_model"), model);
                submesh.material->program()->setMatrix4x4(submesh.material->program()->getUniformLocation("u_viewProjection"), _viewProjection);

                submesh.mesh->bind();
                submesh.mesh->draw();
            }
        }

        _gbuffer->unbind();
    }

    void OpenGLRenderer::lightPass()
    {
        GL_CALL(glDisable(GL_DEPTH_TEST));

        _lightBuffer->bind();
        _quadMesh->bind();

        GL_CALL(glDisable(GL_BLEND));

        // ambient light
        _baseColorProgram->bind();
        _baseColorProgram->setVector4(_baseColorProgram->getUniformLocation("u_color"), glm::vec4(_ambientLight.x, _ambientLight.y, _ambientLight.z, 1));

        _quadMesh->draw();

        // directional light
        GL_CALL(glEnable(GL_BLEND));
        GL_CALL(glBlendFunc(GL_ONE, GL_ONE));

        _gbuffer->colorAttachments()[GBufferNormalsAttachment]->bind(0);
        _gbuffer->depthAttachment()->bind(1);

        glm::mat4 inverseViewProjection = glm::inverse(_viewProjection);

        _directionalLightProgram->bind();
        _directionalLightProgram->setInt(_directionalLightProgram->getUniformLocation("u_normal"), 0);
        _directionalLightProgram->setInt(_directionalLightProgram->getUniformLocation("u_depth"), 1);
        _directionalLightProgram->setVector3(_directionalLightProgram->getUniformLocation("u_direction"), _directionalLightDirection);
        _directionalLightProgram->setVector3(_directionalLightProgram->getUniformLocation("u_color"), _directionalLightColor);
        _directionalLightProgram->setFloat(_directionalLightProgram->getUniformLocation("u_intensity"), _directionalLightIntensity);
        _directionalLightProgram->setMatrix4x4(_directionalLightProgram->getUniformLocation("u_inverseViewProjection"), inverseViewProjection);
        setCameraUniforms(_directionalLightProgram, _camera);

        _quadMesh->draw();

        GL_CALL(glDisable(GL_BLEND));
        _lightBuffer->unbind();

        _gbuffer->colorAttachments()[GBufferNormalsAttachment]->unbind();
        _gbuffer->depthAttachment()->unbind();
    }

    void OpenGLRenderer::finalFramePass()
    {
        GL_CALL(glDisable(GL_DEPTH_TEST));

        if (_bufferToDisplay == BufferToDisplay::finalFrame)
        {
            _gbuffer->colorAttachments()[GBufferAlbedoAttachment]->bind(0);
            _lightBuffer->colorAttachments()[0]->bind(1);

            _combineFinalFrameProgram->bind();
            _combineFinalFrameProgram->setInt(_combineFinalFrameProgram->getUniformLocation("u_albedo"), 0);
            _combineFinalFrameProgram->setInt(_combineFinalFrameProgram->getUniformLocation("u_light"), 1);

            _quadMesh->bind();
            _quadMesh->draw();

            _gbuffer->colorAttachments()[GBufferAlbedoAttachment]->unbind();
            _lightBuffer->colorAttachments()[0]->unbind();
        }
        else
        {
            std::shared_ptr<OpenGLTexture2D> bufferTexture = nullptr;

            if (_bufferToDisplay == BufferToDisplay::lightBuffer)
            {
                bufferTexture = _lightBuffer->colorAttachments()[0];
            }
            else if (_bufferToDisplay == BufferToDisplay::albedo)
            {
                bufferTexture = _gbuffer->colorAttachments()[GBufferAlbedoAttachment];
            }
            else if (_bufferToDisplay == BufferToDisplay::normal)
            {
                bufferTexture = _gbuffer->colorAttachments()[GBufferNormalsAttachment];
            }
            else if (_bufferToDisplay == BufferToDisplay::depth)
            {
                bufferTexture = _gbuffer->depthAttachment();
            }

            ASSERT(bufferTexture != nullptr, "Invalid buffer to display");

            _displayBufferMaterial->setTexture2D("baseColor", bufferTexture);
            _displayBufferMaterial->bind();
            _quadMesh->bind();
            _quadMesh->draw();

            bufferTexture->unbind();
        }
    }

    void OpenGLRenderer::setCameraUniforms(const std::shared_ptr<OpenGLProgram>& program, const std::shared_ptr<PerspectiveCamera>& camera)
    {
        program->setVector3(program->getUniformLocation("u_cameraPosition"), _camera->transform.position);
        program->setVector3(program->getUniformLocation("u_cameraDirection"), _camera->forward());
    }
}
