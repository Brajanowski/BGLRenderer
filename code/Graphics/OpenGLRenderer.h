#pragma once

#include "Gizmos.h"
#include "OpenGLBase.h"
#include "Resources/OpenGLFramebuffer.h"
#include "Resources/OpenGLMaterial.h"
#include "Resources/OpenGLEnvironmentMap.h"
#include "OpenGLRenderObject.h"

#include "EnvironmentMapGenerator.h"

#include <Assets/AssetManager.h>
#include <World/PerspectiveCamera.h>

namespace BGLRenderer
{
    enum class BufferToDisplay
    {
        albedo,
        normal,
        roughness,
        metallic,
        depth,
        lightBuffer,
        finalFrame
    };

    struct OpenGLFrameData
    {
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 viewProjection;

        glm::mat4 viewInv;
        glm::mat4 projectionInv;
        glm::mat4 viewProjectionInv;

        glm::vec2 resolution;
    };

    class OpenGLRenderer
    {
    public:
        OpenGLRenderer(const std::shared_ptr<AssetManager>& assetManager, int frameWidth, int frameHeight);
        ~OpenGLRenderer();

        void onIMGUI();

        void resizeFrame(int width, int height);

        void beginFrame();
        void endFrame();

        inline void setEnvironmentMap(const std::shared_ptr<OpenGLEnvironmentMap>& environmentMap)
        {
            _environmentMap = environmentMap;
        }

        inline void setCamera(const std::shared_ptr<PerspectiveCamera>& camera) { _camera = camera; }

        inline void submit(const std::shared_ptr<OpenGLMaterial>& material,
                           const std::shared_ptr<OpenGLMesh>& mesh,
                           const glm::mat4& model)
        {
            _meshEntries.push_back({material, mesh, model});
        }

        void generateEnvironmentMap(const std::shared_ptr<OpenGLEnvironmentMap>& environmentMap,
                                    const std::shared_ptr<OpenGLTexture2D>& equirectangularMap);

        inline std::shared_ptr<OpenGLMesh> quadMesh() { return _quadMesh; }
        inline Gizmos& gizmos() { return _gizmos; }

        inline const std::string& systemInfo() const { return _systemInfo; }

    private:
        Log _logger{"Renderer"};
        EnvironmentMapGenerator _environmentMapGenerator;

        std::string _systemInfo;

        OpenGLFrameData _frameData;

        std::shared_ptr<AssetManager> _assetManager;
        int _frameWidth;
        int _frameHeight;

        std::shared_ptr<OpenGLTexture2D> _whiteTexture;
        std::shared_ptr<OpenGLTexture2D> _bumpTexture;
        std::shared_ptr<OpenGLMesh> _quadMesh;
        std::shared_ptr<OpenGLProgram> _baseColorProgram;
        std::shared_ptr<OpenGLProgram> _baseTextureProgram;
        std::shared_ptr<OpenGLMaterial> _fallbackMaterial;

        std::shared_ptr<PerspectiveCamera> _camera;

        struct MeshEntry
        {
            std::shared_ptr<OpenGLMaterial> material;
            std::shared_ptr<OpenGLMesh> mesh;
            glm::mat4 model;
        };

        std::vector<MeshEntry> _meshEntries;

        BufferToDisplay _bufferToDisplay = BufferToDisplay::finalFrame;

        std::shared_ptr<OpenGLTexture2D> _frameTexture;

        std::shared_ptr<OpenGLFramebuffer> _gbuffer;
        std::shared_ptr<OpenGLFramebuffer> _lightBuffer;
        std::shared_ptr<OpenGLFramebuffer> _frameFramebuffer;

        std::shared_ptr<OpenGLProgram> _ambientLightProgram;
        std::shared_ptr<OpenGLProgram> _directionalLightProgram;
        std::shared_ptr<OpenGLProgram> _combineFinalFrameProgram;
        std::shared_ptr<OpenGLProgram> _postProcessGammaCorrectionProgram;

        std::shared_ptr<OpenGLProgram> _skyboxProgram;
        std::shared_ptr<OpenGLEnvironmentMap> _environmentMap;

        glm::vec3 _ambientLight = {0, 0, 0};
        glm::vec3 _directionalLightDirection = {0.2f, -0.5f, -1.0f};
        glm::vec3 _directionalLightColor = {1.0f, 1.0f, 1.0f};
        float _directionalLightIntensity = 1.0f;

        // Debug stuff
        Gizmos _gizmos{};
        std::shared_ptr<OpenGLProgram> _textureChannelProgram;

        bool _postProcess = true;

        void initializeDefaultResources();

        void skyboxPass();

        void gbufferPass();
        void ambientLightPass();
        void lightPass();
        void combineLighting();
        void unlitPass();
        void presentFinalFrame();

        void setCameraUniforms(const std::shared_ptr<OpenGLProgram>& program,
                               const std::shared_ptr<PerspectiveCamera>& camera);

        void renderMeshEntries(MaterialType materialType);

        static void setFrameDataUniforms(const std::shared_ptr<OpenGLProgram>& program,
                                         const OpenGLFrameData& frameData);
    };
}
