#pragma once

#include "OpenGLBase.h"
#include "OpenGLMaterial.h"
#include "OpenGLRenderObject.h"

namespace BGLRenderer
{
    class OpenGLRenderer
    {
    public:
        OpenGLRenderer(int frameWidth, int frameHeight);
        ~OpenGLRenderer();

        void resizeFrame(int width, int height);

        void beginFrame();
        void endFrame();

        inline void setViewProjectionMatrix(const glm::mat4& viewProjection) { _viewProjection = viewProjection; }
        inline void submit(const std::shared_ptr<OpenGLRenderObject>& renderObject) { _renderObjects.push_back(renderObject); }

        inline std::shared_ptr<OpenGLMesh> quadMesh() { return _quadMesh; }

    private:
        int _frameWidth;
        int _frameHeight;

        std::shared_ptr<OpenGLMesh> _quadMesh;

        glm::mat4 _viewProjection;
        std::vector<std::shared_ptr<OpenGLRenderObject>> _renderObjects;

        void initializeDefaultResources();
    };
}
