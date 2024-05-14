#include "OpenGLRenderer.h"

namespace BGLRenderer
{
    OpenGLRenderer::OpenGLRenderer(int frameWidth, int frameHeight) :
        _frameWidth(frameWidth),
        _frameHeight(frameHeight)
    {
        
    }

    OpenGLRenderer::~OpenGLRenderer()
    {
    }

    void OpenGLRenderer::resizeFrame(int width, int height)
    {
        _frameWidth = width;
        _frameHeight = height;
    }

    void OpenGLRenderer::beginFrame()
    {
        glViewport(0, 0, _frameWidth, _frameHeight);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRenderer::render()
    {
        
    }
}
