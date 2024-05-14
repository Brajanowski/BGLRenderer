#pragma once

#include "OpenGLBase.h"
#include "OpenGLMaterial.h"

namespace BGLRenderer
{
    class OpenGLRenderer
    {
    public:
        OpenGLRenderer(int frameWidth, int frameHeight);
        ~OpenGLRenderer();

        void resizeFrame(int width, int height);

        void beginFrame();
        void render();

    private:
        int _frameWidth;
        int _frameHeight;
    };
}
