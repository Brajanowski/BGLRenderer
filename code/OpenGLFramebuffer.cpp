#include "OpenGLFramebuffer.h"

namespace BGLRenderer
{
    OpenGLFramebuffer::OpenGLFramebuffer(GLuint width, GLuint height) :
        _width(width),
        _height(height)
    {
        GL_CALL(glGenFramebuffers(1, &_id));
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, _id));
    }

    OpenGLFramebuffer::~OpenGLFramebuffer()
    {
        GL_CALL(glDeleteFramebuffers(1, &_id));
    }

    int OpenGLFramebuffer::createColorAttachment(GLenum format)
    {
        std::shared_ptr<OpenGLTexture2D> texture = std::make_shared<OpenGLTexture2D>(_width, _height, format, GL_CLAMP_TO_EDGE, GL_LINEAR);

        texture->bind();
        texture->generatePixelsBuffer();

        bind();
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)(GL_COLOR_ATTACHMENT0 + _colorAttachments.size()), GL_TEXTURE_2D, texture->id(), 0));

        _colorAttachments.push_back(texture);
        return _colorAttachments.size() - 1;
    }

    void OpenGLFramebuffer::createDepthAttachment(GLenum format)
    {
        std::shared_ptr<OpenGLTexture2D> texture = std::make_shared<OpenGLTexture2D>(_width, _height, format, GL_CLAMP_TO_EDGE, GL_LINEAR);

        texture->bind();
        texture->generatePixelsBuffer();

        bind();
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->id(), 0));

        _depthAttachment = texture;
    }

    bool OpenGLFramebuffer::validate()
    {
        bind();

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            openGLLogger.error("Framebuffer is not complete! Status: %d", status);
            unbind();
            return false;
        }

        unbind();
        return true;
    }

    void OpenGLFramebuffer::bind()
    {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, _id));
    }

    void OpenGLFramebuffer::unbind()
    {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void OpenGLFramebuffer::resize(GLuint width, GLuint height)
    {
        for (const auto& texture : _colorAttachments)
        {
            texture->resize(width, height);
        }

        if (_depthAttachment != nullptr)
        {
            _depthAttachment->resize(width, height);
        }
    }

    void OpenGLFramebuffer::bindAttachments()
    {
        for (int i = 0; i < _colorAttachments.size(); ++i)
        {
            _colorAttachments[i]->bind(i);
        }
    }
}
