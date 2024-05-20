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

        inline void submit(const std::shared_ptr<OpenGLRenderObject>& renderObject) { _renderObjects.push_back(renderObject); }

    private:
        int _frameWidth;
        int _frameHeight;

        std::vector<std::shared_ptr<OpenGLRenderObject>> _renderObjects;
    };
}
