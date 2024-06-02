#include "OpenGLRenderer.h"

#include <imgui.h>

namespace BGLRenderer
{
    constexpr int GBufferAlbedoAttachment = 0;
    constexpr int GBufferNormalsAttachment = 1;

    OpenGLRenderer::OpenGLRenderer(const std::shared_ptr<AssetManager>& assetsLoader, int frameWidth, int frameHeight) :
        _assetManager(assetsLoader),
        _frameWidth(frameWidth),
        _frameHeight(frameHeight)
    {
        initializeDefaultResources();

        _systemInfo += "Vendor: ";
        _systemInfo += reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        _systemInfo += '\n';

        _systemInfo += "Renderer: ";
        _systemInfo += reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        _systemInfo += '\n';

        _systemInfo += "Version: ";
        _systemInfo += reinterpret_cast<const char*>(glGetString(GL_VERSION));
        _systemInfo += '\n';

        _systemInfo += "Shading Language Version: ";
        _systemInfo += reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
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

        _frameFramebuffer->resize(width, height);
        _gbuffer->resize(width, height);
        _lightBuffer->resize(width, height);
    }

    void OpenGLRenderer::beginFrame()
    {
        _meshEntries.clear();
    }

    void OpenGLRenderer::endFrame()
    {
        _viewProjection = _camera->viewProjection();

        gbufferPass();
        lightPass();
        combineLighting();
        unlitPass();
        presentFinalFrame();
    }

    void OpenGLRenderer::initializeDefaultResources()
    {
        _logger.debug("Initializing default resources");

        constexpr int whiteTextureSize = 2;

        GLubyte whiteColors[whiteTextureSize * whiteTextureSize];
        std::memset(whiteColors, 0xFF, sizeof(GLubyte) * whiteTextureSize * whiteTextureSize);

        _whiteTexture = std::make_shared<OpenGLTexture2D>("white", whiteTextureSize, whiteTextureSize, GL_RGBA, GL_REPEAT, GL_LINEAR);
        _whiteTexture->setPixels(GL_RGB, whiteColors);
        _assetManager->registerAsset("white", _whiteTexture);

        _fallbackMaterial = std::make_shared<OpenGLMaterial>("fallback", MaterialType::unlit, _assetManager->getProgram("shaders/fallback"));
        _assetManager->registerAsset("fallback", _fallbackMaterial);

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

        _baseColorProgram = _assetManager->getProgram("shaders/base_color");
        _baseTextureProgram = _assetManager->getProgram("shaders/basic");

        // GBuffer - do not change the order of attachments
        _gbuffer = std::make_shared<OpenGLFramebuffer>("GBuffer", _frameWidth, _frameHeight);
        _gbuffer->createColorAttachment("Albedo", GL_RGBA);
        _gbuffer->createColorAttachment("Normal", GL_RGBA);
        _gbuffer->createDepthAttachment(GL_DEPTH_COMPONENT);
        _gbuffer->validate();

        _frameTexture = std::make_shared<OpenGLTexture2D>("Frame Texture", _frameWidth, _frameHeight, GL_RGBA);
        _frameTexture->generatePixelsBuffer();

        _lightBuffer = std::make_shared<OpenGLFramebuffer>("Light Buffer", _frameWidth, _frameHeight);
        _lightBuffer->createColorAttachment("Light", GL_RGB);
        //_lightBuffer->addColorAttachment(_frameTexture);
        _lightBuffer->validate();

        _frameFramebuffer = std::make_shared<OpenGLFramebuffer>("Frame Framebuffer", _frameWidth, _frameHeight);
        _frameFramebuffer->addColorAttachment(_frameTexture, true);
        //_frameFramebuffer->createColorAttachment("Frame", GL_RGBA);
        _frameFramebuffer->setDepthAttachment(_gbuffer->depthAttachment().texture);
        _frameFramebuffer->validate();

        _ambientLightProgram = _assetManager->getProgram("shaders/light_ambient");
        _directionalLightProgram = _assetManager->getProgram("shaders/light_directional");
        _combineFinalFrameProgram = _assetManager->getProgram("shaders/combine_finalframe");
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

        GL_CALL(glDepthMask(GL_TRUE));
        GL_CALL(glEnable(GL_DEPTH_TEST));
        GL_CALL(glDepthFunc(GL_LEQUAL));
        GL_CALL(glEnable(GL_CULL_FACE));
        GL_CALL(glCullFace(GL_BACK));

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderMeshEntries(MaterialType::opaque);

        _gbuffer->unbind();
    }

    void OpenGLRenderer::lightPass()
    {
        _lightBuffer->bind();
        _quadMesh->bind();

        GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

        GL_CALL(glDepthMask(GL_FALSE));

        GL_CALL(glDisable(GL_DEPTH_TEST));
        GL_CALL(glDisable(GL_BLEND));

        _gbuffer->depthAttachment().texture->bind(3);

        // ambient light
        _ambientLightProgram->bind();
        _ambientLightProgram->setVector4(_ambientLightProgram->getUniformLocation("u_ambientColor"), glm::vec4(_ambientLight.x, _ambientLight.y, _ambientLight.z, 1));
        _ambientLightProgram->setInt(_ambientLightProgram->getUniformLocation("u_depth"), 3);

        _quadMesh->draw();

        // directional light
        GL_CALL(glEnable(GL_BLEND));
        GL_CALL(glBlendFunc(GL_ONE, GL_ONE));

        //_gbuffer->colorAttachments()[GBufferAlbedoAttachment].texture->bind(0);
        _gbuffer->colorAttachments()[GBufferNormalsAttachment].texture->bind(1);
        // _gbuffer->colorAttachments()[GBufferAlbedoAttachment].texture->bind(2); - surface details
        //_gbuffer->depthAttachment().texture->bind(3);

        glm::mat4 inverseViewProjection = glm::inverse(_viewProjection);

        _directionalLightProgram->bind();
        //_directionalLightProgram->setInt(_directionalLightProgram->getUniformLocation("u_albedo"), 0);
        _directionalLightProgram->setInt(_directionalLightProgram->getUniformLocation("u_normal"), 1);
        _directionalLightProgram->setInt(_directionalLightProgram->getUniformLocation("u_depth"), 3);
        _directionalLightProgram->setVector3(_directionalLightProgram->getUniformLocation("u_direction"), _directionalLightDirection);
        _directionalLightProgram->setVector3(_directionalLightProgram->getUniformLocation("u_color"), _directionalLightColor);
        _directionalLightProgram->setFloat(_directionalLightProgram->getUniformLocation("u_intensity"), _directionalLightIntensity);
        _directionalLightProgram->setMatrix4x4(_directionalLightProgram->getUniformLocation("u_inverseViewProjection"), inverseViewProjection);
        setCameraUniforms(_directionalLightProgram, _camera);

        _quadMesh->draw();

        GL_CALL(glDisable(GL_BLEND));
        _lightBuffer->unbind();

        _gbuffer->colorAttachments()[GBufferNormalsAttachment].texture->unbind();
        _gbuffer->depthAttachment().texture->unbind();
    }

    void OpenGLRenderer::unlitPass()
    {
        _frameFramebuffer->bind();

        GL_CALL(glDisable(GL_BLEND));

        GL_CALL(glDepthMask(GL_TRUE));
        GL_CALL(glEnable(GL_DEPTH_TEST));
        GL_CALL(glDepthFunc(GL_LEQUAL));

        GL_CALL(glEnable(GL_CULL_FACE));
        GL_CALL(glCullFace(GL_BACK));

        renderMeshEntries(MaterialType::unlit);

        _frameFramebuffer->unbind();
    }

    void OpenGLRenderer::combineLighting()
    {
        GL_CALL(glDisable(GL_DEPTH_TEST));

        _frameFramebuffer->bind();

        GLenum drawTargets[] = {
            GL_COLOR_ATTACHMENT0
        };
        GL_CALL(glDrawBuffers(1, drawTargets));

        _gbuffer->colorAttachments()[GBufferAlbedoAttachment].texture->bind(0);
        _lightBuffer->colorAttachments()[0].texture->bind(1);

        _combineFinalFrameProgram->bind();
        _combineFinalFrameProgram->setInt(_combineFinalFrameProgram->getUniformLocation("u_albedo"), 0);
        _combineFinalFrameProgram->setInt(_combineFinalFrameProgram->getUniformLocation("u_light"), 1);

        _quadMesh->bind();
        _quadMesh->draw();

        _gbuffer->colorAttachments()[GBufferAlbedoAttachment].texture->unbind();
        _lightBuffer->colorAttachments()[0].texture->unbind();

        _frameFramebuffer->unbind();
    }

    void OpenGLRenderer::presentFinalFrame()
    {
        GL_CALL(glDisable(GL_DEPTH_TEST));
        GL_CALL(glDisable(GL_CULL_FACE));

        glViewport(0, 0, _frameWidth, _frameHeight);
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        std::shared_ptr<OpenGLTexture2D> bufferTexture = nullptr;

        if (_bufferToDisplay == BufferToDisplay::finalFrame)
        {
            bufferTexture = _frameTexture;
        }
        else if (_bufferToDisplay == BufferToDisplay::lightBuffer)
        {
            bufferTexture = _lightBuffer->colorAttachments()[0].texture;
        }
        else if (_bufferToDisplay == BufferToDisplay::albedo)
        {
            bufferTexture = _gbuffer->colorAttachments()[GBufferAlbedoAttachment].texture;
        }
        else if (_bufferToDisplay == BufferToDisplay::normal)
        {
            bufferTexture = _gbuffer->colorAttachments()[GBufferNormalsAttachment].texture;
        }
        else if (_bufferToDisplay == BufferToDisplay::depth)
        {
            bufferTexture = _gbuffer->depthAttachment().texture;
        }

        ASSERT(bufferTexture != nullptr, "Invalid buffer to display");

        bufferTexture->bind(0);

        _baseTextureProgram->bind();
        _baseTextureProgram->setVector4(_baseTextureProgram->getUniformLocation("u_tint"), {1, 1, 1, 1});
        _baseTextureProgram->setInt(_baseTextureProgram->getUniformLocation("u_baseColor"), 0);
        _baseTextureProgram->setMatrix4x4(_baseTextureProgram->getUniformLocation("u_model"), glm::mat4(1.0f));
        _baseTextureProgram->setMatrix4x4(_baseTextureProgram->getUniformLocation("u_viewProjection"), glm::mat4(1.0f));

        _quadMesh->bind();
        _quadMesh->draw();

        bufferTexture->unbind();
    }

    void OpenGLRenderer::setCameraUniforms(const std::shared_ptr<OpenGLProgram>& program, const std::shared_ptr<PerspectiveCamera>& camera)
    {
        program->setVector3(program->getUniformLocation("u_cameraPosition"), camera->transform.position);
        program->setVector3(program->getUniformLocation("u_cameraDirection"), camera->forward());
    }

    void OpenGLRenderer::renderMeshEntries(MaterialType materialType)
    {
        for (const auto& meshEntry : _meshEntries)
        {
            std::shared_ptr<OpenGLMaterial> material = meshEntry.material;
            if (material == nullptr || !material->valid())
            {
                material = _fallbackMaterial;
            }

            if (material->type() != materialType)
            {
                continue;
            }

            material->bind();
            material->program()->setVector2(material->program()->getUniformLocation("u_resolution"), glm::vec2{static_cast<float>(_frameWidth), static_cast<float>(_frameHeight)});
            material->program()->setMatrix4x4(material->program()->getUniformLocation("u_model"), meshEntry.model);
            material->program()->setMatrix4x4(material->program()->getUniformLocation("u_viewProjection"), _viewProjection);

            meshEntry.mesh->bind();
            meshEntry.mesh->draw();
        }
    }
}
