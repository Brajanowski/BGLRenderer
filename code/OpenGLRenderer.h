#pragma once

#include "AssetsLoader.h"
#include "OpenGLBase.h"
#include "OpenGLFramebuffer.h"
#include "OpenGLMaterial.h"
#include "OpenGLRenderObject.h"
#include "PerspectiveCamera.h"

namespace BGLRenderer
{
    enum class BufferToDisplay
    {
        albedo,
        normal,
        depth,
        lightBuffer,
        finalFrame
    };
    
    class OpenGLRenderer
    {
    public:
        OpenGLRenderer(const std::shared_ptr<AssetsLoader>& assetsLoader, int frameWidth, int frameHeight);
        ~OpenGLRenderer();

        void onIMGUI();

        void resizeFrame(int width, int height);

        void beginFrame();
        void endFrame();

        inline void setCamera(const std::shared_ptr<PerspectiveCamera>& camera) { _camera = camera; }
        inline void submit(const std::shared_ptr<OpenGLRenderObject>& renderObject) { _renderObjects.push_back(renderObject); }

        inline std::shared_ptr<OpenGLMesh> quadMesh() { return _quadMesh; }

    private:
        Log _logger{"Renderer"};

        std::shared_ptr<AssetsLoader> _assetsLoader;
        int _frameWidth;
        int _frameHeight;

        std::shared_ptr<OpenGLMesh> _quadMesh;
        std::shared_ptr<OpenGLProgram> _baseColorProgram;

        std::shared_ptr<PerspectiveCamera> _camera;
        glm::mat4 _viewProjection;
        std::vector<std::shared_ptr<OpenGLRenderObject>> _renderObjects;

        BufferToDisplay _bufferToDisplay = BufferToDisplay::finalFrame;
        
        std::shared_ptr<OpenGLFramebuffer> _gbuffer;
        std::shared_ptr<OpenGLFramebuffer> _lightBuffer;

        std::shared_ptr<OpenGLProgram> _directionalLightProgram;
        std::shared_ptr<OpenGLProgram> _combineFinalFrameProgram;
        
        std::shared_ptr<OpenGLMaterial> _displayBufferMaterial;

        glm::vec3 _ambientLight = {0.25f, 0.25f, 0.25f};
        glm::vec3 _directionalLightDirection = {0.2f, -0.5f, 1.0f};
        glm::vec3 _directionalLightColor = {1.0f, 1.0f, 1.0f};
        float _directionalLightIntensity = 1.0f;

        void initializeDefaultResources();

        void gbufferPass();
        void lightPass();
        void finalFramePass();

        void setCameraUniforms(const std::shared_ptr<OpenGLProgram>& program, const std::shared_ptr<PerspectiveCamera>& camera);
    };
}
