#include "OpenGLRenderer.h"

#include <../../lib/ImGui/imgui.h>

namespace BGLRenderer
{
    constexpr int GBufferAlbedoAttachment = 0;
    constexpr int GBufferNormalsAttachment = 1;
    constexpr int GBufferSurfaceAttachment = 2;

    OpenGLRenderer::OpenGLRenderer(const std::shared_ptr<AssetManager>& assetManager, int frameWidth, int frameHeight) :
        _environmentMapGenerator(this, assetManager),
        _assetManager(assetManager),
        _frameWidth(frameWidth),
        _frameHeight(frameHeight)
    {
        initializeDefaultResources();

        _systemInfo += "Vendor: ";
        _systemInfo += reinterpret_cast<const char *>(glGetString(GL_VENDOR));
        _systemInfo += '\n';

        _systemInfo += "Renderer: ";
        _systemInfo += reinterpret_cast<const char *>(glGetString(GL_RENDERER));
        _systemInfo += '\n';

        _systemInfo += "Version: ";
        _systemInfo += reinterpret_cast<const char *>(glGetString(GL_VERSION));
        _systemInfo += '\n';

        _systemInfo += "Shading Language Version: ";
        _systemInfo += reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
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
            "Roughness",
            "Metallic",
            "Depth",
            "Light"
        };
        static BufferToDisplay bufferToDisplayValues[] = {
            BufferToDisplay::finalFrame,
            BufferToDisplay::albedo,
            BufferToDisplay::normal,
            BufferToDisplay::roughness,
            BufferToDisplay::metallic,
            BufferToDisplay::depth,
            BufferToDisplay::lightBuffer
        };
        static constexpr int bufferToDisplayCount = sizeof(bufferToDisplayStrings) / sizeof(const char *);

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

        ImGui::Checkbox("Post Processing", &_postProcess);

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
        _frameData.view = _camera->view();
        _frameData.projection = _camera->projection();
        _frameData.viewProjection = _frameData.projection * _frameData.view;
        _frameData.viewInv = glm::inverse(_frameData.view);
        _frameData.projectionInv = glm::inverse(_frameData.projection);
        _frameData.viewProjectionInv = glm::inverse(_frameData.viewProjection);
        _frameData.resolution = glm::vec2(static_cast<float>(_frameWidth),
                                          static_cast<float>(_frameHeight));

        gbufferPass();
        ambientLightPass();
        lightPass();
        combineLighting();
        unlitPass();
        skyboxPass();
        presentFinalFrame();

        GL_CALL(glDepthMask(GL_TRUE));
        GL_CALL(glEnable(GL_DEPTH_TEST));
        GL_CALL(glDepthFunc(GL_LEQUAL));
        GL_CALL(glEnable(GL_CULL_FACE));
        GL_CALL(glCullFace(GL_BACK));

        GL_CALL(glClear(GL_DEPTH_BUFFER_BIT));

        _gizmos.render(_frameData.viewProjection);
    }

    void OpenGLRenderer::generateEnvironmentMap(const std::shared_ptr<OpenGLEnvironmentMap>& environmentMap,
                                                const std::shared_ptr<OpenGLTexture2D>& equirectangularMap)
    {
        _environmentMapGenerator.generate(environmentMap, equirectangularMap);
    }

    void OpenGLRenderer::initializeDefaultResources()
    {
        _logger.debug("Initializing default resources");

        // preload some shaders
        _assetManager->getProgram("shaders/fallback");
        _assetManager->getProgram("shaders/gbuffer_default");

        // textures
        constexpr int whiteTextureSize = 2;
        GLubyte whiteColors[whiteTextureSize * whiteTextureSize * 4];
        std::memset(whiteColors, 0xFF, sizeof(GLubyte) * whiteTextureSize * whiteTextureSize * 4);

        _whiteTexture = std::make_shared<OpenGLTexture2D>("white", whiteTextureSize, whiteTextureSize, GL_RGBA,
                                                          WrapMode::repeat, FilterMode::pixel);
        _whiteTexture->setPixels(GL_RGBA, whiteColors);
        _assetManager->registerAsset("white", _whiteTexture);

        GLubyte bumpColors[whiteTextureSize * whiteTextureSize * 4];
        for (std::size_t i = 0; i < 4; ++i)
        {
            bumpColors[i * 4] = 0x80;
            bumpColors[i * 4 + 1] = 0x80;
            bumpColors[i * 4 + 2] = 0xFF;
            bumpColors[i * 4 + 3] = 0xFF;
        }
        _bumpTexture = std::make_shared<OpenGLTexture2D>("bump", whiteTextureSize, whiteTextureSize, GL_RGBA,
                                                         WrapMode::repeat, FilterMode::pixel);
        _bumpTexture->setPixels(GL_RGBA, bumpColors);
        _assetManager->registerAsset("bump", _bumpTexture);

        // default materials
        _fallbackMaterial = std::make_shared<OpenGLMaterial>("fallback", MaterialType::unlit, MaterialTag::none,
                                                             _assetManager->getProgram("shaders/fallback"));
        _assetManager->registerAsset("fallback", _fallbackMaterial);

        _quadMesh = std::make_shared<OpenGLMesh>();

        std::vector<GLfloat> quadPositions = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
        };

        std::vector<GLfloat> normals = {
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f
        };

        std::vector<GLfloat> uvs = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };

        std::vector<GLuint> indices = {
            0, 1, 2,
            2, 3, 0
        };

        std::vector<GLfloat> quadTangents = {
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f
        };

        //OpenGLMesh::calculateTangents(quadTangents, quadPositions, normals, uvs, indices);

        _quadMesh->setVertices(quadPositions.data(), static_cast<GLuint>(quadPositions.size()));
        _quadMesh->setNormals(normals.data(), static_cast<GLuint>(normals.size()));
        _quadMesh->setUVs0(uvs.data(), static_cast<GLuint>(uvs.size()));
        _quadMesh->setIndices(indices.data(), static_cast<GLuint>(indices.size()));
        _quadMesh->setTangents(quadTangents.data(), static_cast<GLuint>(quadTangents.size()));

        _baseColorProgram = _assetManager->getProgram("shaders/base_color");
        _baseTextureProgram = _assetManager->getProgram("shaders/basic");

        // GBuffer - do not change the order of attachments
        _gbuffer = std::make_shared<OpenGLFramebuffer>("GBuffer", _frameWidth, _frameHeight);
        _gbuffer->createColorAttachment("Albedo", GL_RGBA);
        _gbuffer->createColorAttachment("Normal", GL_RGBA);
        _gbuffer->createColorAttachment("Surface", GL_RGBA);
        _gbuffer->createDepthAttachment(GL_DEPTH_COMPONENT);
        _gbuffer->validate();

        _frameTexture = std::make_shared<OpenGLTexture2D>("Frame Texture", _frameWidth, _frameHeight, GL_RGBA32F);
        _frameTexture->generatePixelsBuffer();

        _lightBuffer = std::make_shared<OpenGLFramebuffer>("Light Buffer", _frameWidth, _frameHeight);
        _lightBuffer->createColorAttachment("Light", GL_RGBA32F);
        _lightBuffer->setDepthAttachment(_gbuffer->depthAttachment().texture);
        _lightBuffer->validate();

        _skyboxProgram = _assetManager->getProgram("shaders/skybox");

        _frameFramebuffer = std::make_shared<OpenGLFramebuffer>("Frame Framebuffer", _frameWidth, _frameHeight);
        _frameFramebuffer->addColorAttachment(_frameTexture, true);
        _frameFramebuffer->setDepthAttachment(_gbuffer->depthAttachment().texture);
        _frameFramebuffer->validate();

        _ambientLightProgram = _assetManager->getProgram("shaders/light_ambient");
        _directionalLightProgram = _assetManager->getProgram("shaders/light_directional");
        _combineFinalFrameProgram = _assetManager->getProgram("shaders/combine_finalframe");
        _postProcessGammaCorrectionProgram = _assetManager->getProgram("shaders/postprocess.vert",
                                                                       "shaders/postprocess_gammacorrection.frag");

        // debug stuff
        _textureChannelProgram = _assetManager->getProgram("shaders/debug_texture.vert",
                                                           "shaders/debug_texture_channel.frag");

        GizmosAssets gizmosAssets{};
        gizmosAssets.gizmosProgram = _assetManager->getProgram("shaders/gizmos");
        gizmosAssets.arrow = _assetManager->getModel("gizmos/arrow.gltf", gizmosAssets.gizmosProgram)->submeshes()[0].
                mesh;
        gizmosAssets.sphere = _assetManager->getModel("sphere_hres.gltf", gizmosAssets.gizmosProgram)->submeshes()[0].
                mesh;
        gizmosAssets.wireCube = _assetManager->getModel("gizmos/wire_cube.gltf", gizmosAssets.gizmosProgram)->
                submeshes()[0].mesh;
        _gizmos.setAssets(gizmosAssets);
    }

    void OpenGLRenderer::skyboxPass()
    {
        _frameFramebuffer->bind();
        glViewport(0, 0, _frameWidth, _frameHeight);

        _environmentMap->cubemap()->bind(0);

        _skyboxProgram->bind();
        _skyboxProgram->setInt("u_texture", 0);

        glm::mat4 viewNoTranslate = _frameData.view;
        viewNoTranslate[3][0] = 0.0f;
        viewNoTranslate[3][1] = 0.0f;
        viewNoTranslate[3][2] = 0.0f;
        viewNoTranslate[3][1] = 1.0f;

        glm::mat4 viewProjectionInv = glm::inverse(_frameData.projection * viewNoTranslate);
        _skyboxProgram->setMatrix4x4("u_viewProjectionInv", viewProjectionInv);

        GL_CALL(glEnable(GL_DEPTH_TEST));
        GL_CALL(glDepthMask(GL_FALSE));
        GL_CALL(glDepthFunc(GL_LEQUAL));

        _quadMesh->bind();
        _quadMesh->draw();

        _frameFramebuffer->unbind();
    }

    void OpenGLRenderer::gbufferPass()
    {
        _gbuffer->bind();

        GLenum drawTargets[] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2
        };
        GL_CALL(glDrawBuffers(3, drawTargets));

        glViewport(0, 0, _frameWidth, _frameHeight);

        GL_CALL(glDepthMask(GL_TRUE));
        GL_CALL(glEnable(GL_DEPTH_TEST));
        GL_CALL(glDepthFunc(GL_LEQUAL));
        GL_CALL(glEnable(GL_CULL_FACE));
        GL_CALL(glCullFace(GL_BACK));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderMeshEntries(MaterialType::opaque);

        _gbuffer->unbind();
    }

    void OpenGLRenderer::ambientLightPass()
    {
        _lightBuffer->bind();

        glViewport(0, 0, _frameWidth, _frameHeight);

        GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

        GL_CALL(glDisable(GL_BLEND));
        GL_CALL(glDisable(GL_DEPTH_TEST));

        _gbuffer->colorAttachments()[GBufferAlbedoAttachment].texture->bind(0);
        _gbuffer->colorAttachments()[GBufferNormalsAttachment].texture->bind(1);
        _gbuffer->colorAttachments()[GBufferSurfaceAttachment].texture->bind(2);
        _gbuffer->depthAttachment().texture->bind(3);
        _environmentMap->irradianceMap()->bind(4);

        _ambientLightProgram->bind();
        _ambientLightProgram->setInt("u_albedo", 0);
        _ambientLightProgram->setInt("u_normal", 1);
        _ambientLightProgram->setInt("u_surface", 2);
        _ambientLightProgram->setInt("u_depth", 3);
        _ambientLightProgram->setInt("u_irradiance", 4);

        setFrameDataUniforms(_ambientLightProgram, _frameData);
        setCameraUniforms(_ambientLightProgram, _camera);

        _quadMesh->bind();
        _quadMesh->draw();

        _lightBuffer->unbind();
    }

    void OpenGLRenderer::lightPass()
    {
        _lightBuffer->bind();

        glViewport(0, 0, _frameWidth, _frameHeight);

        // directional light
        GL_CALL(glEnable(GL_BLEND));
        GL_CALL(glBlendFunc(GL_ONE, GL_ONE));

        _gbuffer->colorAttachments()[GBufferAlbedoAttachment].texture->bind(0);
        _gbuffer->colorAttachments()[GBufferNormalsAttachment].texture->bind(1);
        _gbuffer->colorAttachments()[GBufferSurfaceAttachment].texture->bind(2);
        _gbuffer->depthAttachment().texture->bind(3);

        _environmentMap->cubemap()->bind(4);

        _directionalLightProgram->bind();
        _directionalLightProgram->setInt("u_albedo", 0);
        _directionalLightProgram->setInt("u_normal", 1);
        _directionalLightProgram->setInt("u_surface", 2);
        _directionalLightProgram->setInt("u_depth", 3);
        _directionalLightProgram->setInt("u_skyTexture", 4);
        _directionalLightProgram->setVector3("u_direction", _directionalLightDirection);
        _directionalLightProgram->setVector3("u_color", _directionalLightColor);
        _directionalLightProgram->setFloat("u_intensity", _directionalLightIntensity);
        setFrameDataUniforms(_directionalLightProgram, _frameData);
        setCameraUniforms(_directionalLightProgram, _camera);

        _quadMesh->bind();
        _quadMesh->draw();

        GL_CALL(glDisable(GL_BLEND));
        _lightBuffer->unbind();

        _gbuffer->colorAttachments()[GBufferNormalsAttachment].texture->unbind();
        _gbuffer->depthAttachment().texture->unbind();
    }

    void OpenGLRenderer::unlitPass()
    {
        _frameFramebuffer->bind();

        glViewport(0, 0, _frameWidth, _frameHeight);

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

        glViewport(0, 0, _frameWidth, _frameHeight);

        constexpr GLenum drawTargets[] = {
            GL_COLOR_ATTACHMENT0
        };
        GL_CALL(glDrawBuffers(1, drawTargets));

        _gbuffer->colorAttachments()[GBufferAlbedoAttachment].texture->bind(0);
        _lightBuffer->colorAttachments()[0].texture->bind(1);

        _combineFinalFrameProgram->bind();
        _combineFinalFrameProgram->setInt("u_albedo", 0);
        _combineFinalFrameProgram->setInt("u_light", 1);

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

            if (_postProcess)
            {
                bufferTexture->bind(0);

                _postProcessGammaCorrectionProgram->bind();
                _postProcessGammaCorrectionProgram->setInt("u_texture", 0);

                _quadMesh->bind();
                _quadMesh->draw();

                bufferTexture->unbind();

                return;
            }
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
        else if (_bufferToDisplay == BufferToDisplay::roughness)
        {
            bufferTexture = _gbuffer->colorAttachments()[GBufferSurfaceAttachment].texture;

            bufferTexture->bind(0);

            _textureChannelProgram->bind();
            _textureChannelProgram->setInt(_textureChannelProgram->getUniformLocation("u_texture"), 0);
            _textureChannelProgram->setVector4(_textureChannelProgram->getUniformLocation("u_channel"),
                                               glm::vec4{1, 0, 0, 0});

            _quadMesh->bind();
            _quadMesh->draw();

            bufferTexture->unbind();
            return;
        }
        else if (_bufferToDisplay == BufferToDisplay::metallic)
        {
            bufferTexture = _gbuffer->colorAttachments()[GBufferSurfaceAttachment].texture;

            bufferTexture->bind(0);

            _textureChannelProgram->bind();
            _textureChannelProgram->setInt("u_texture", 0);
            _textureChannelProgram->setVector4("u_channel", glm::vec4{0, 1, 0, 0});

            _quadMesh->bind();
            _quadMesh->draw();

            bufferTexture->unbind();
            return;
        }
        else if (_bufferToDisplay == BufferToDisplay::depth)
        {
            bufferTexture = _gbuffer->depthAttachment().texture;
        }

        ASSERT(bufferTexture != nullptr, "Invalid buffer to display");

        bufferTexture->bind(0);

        _baseTextureProgram->bind();
        _baseTextureProgram->setVector4("u_tint", {1, 1, 1, 1});
        _baseTextureProgram->setInt("u_baseColor", 0);
        _baseTextureProgram->setMatrix4x4("u_model", glm::mat4(1.0f));
        _baseTextureProgram->setMatrix4x4("u_viewProjection", glm::mat4(1.0f));

        _quadMesh->bind();
        _quadMesh->draw();

        bufferTexture->unbind();
    }

    void OpenGLRenderer::setCameraUniforms(const std::shared_ptr<OpenGLProgram>& program,
                                           const std::shared_ptr<PerspectiveCamera>& camera)
    {
        program->setVector3("u_cameraPosition", camera->transform.position);
        program->setVector3("u_cameraDirection", camera->forward());
    }

    void OpenGLRenderer::renderMeshEntries(MaterialType materialType)
    {
        for (const auto& meshEntry: _meshEntries)
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

            material->program()->setMatrix4x4("u_model", meshEntry.model);
            setFrameDataUniforms(material->program(), _frameData);

            meshEntry.mesh->bind();
            meshEntry.mesh->draw();
        }
    }

    // @TODO - move this stuff to uniform buffer instead of setting these values per program
    void OpenGLRenderer::setFrameDataUniforms(const std::shared_ptr<OpenGLProgram>& program,
                                              const OpenGLFrameData& frameData)
    {
        program->setMatrix4x4("u_view", frameData.view);
        program->setMatrix4x4("u_projection", frameData.projection);
        program->setMatrix4x4("u_viewProjection", frameData.viewProjection);

        program->setMatrix4x4("u_viewInv", frameData.viewInv);
        program->setMatrix4x4("u_projectionInv", frameData.projectionInv);
        program->setMatrix4x4("u_viewProjectionInv", frameData.viewProjectionInv);

        program->setVector2("u_resolution", frameData.resolution);
    }
}
