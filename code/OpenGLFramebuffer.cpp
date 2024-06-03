#include "OpenGLFramebuffer.h"

namespace BGLRenderer
{
    OpenGLFramebuffer::OpenGLFramebuffer(const std::string& name, GLuint width, GLuint height) :
        _name(name),
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

    int OpenGLFramebuffer::addColorAttachment(const std::shared_ptr<OpenGLTexture2D>& texture, bool autoResize)
    {
        texture->bind();
        texture->generatePixelsBuffer();

        bind();
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)(GL_COLOR_ATTACHMENT0 + _colorAttachments.size()), GL_TEXTURE_2D, texture->id(), 0));

        _colorAttachments.push_back({texture, autoResize});
        return static_cast<int>(_colorAttachments.size() - 1);
    }

    int OpenGLFramebuffer::createColorAttachment(const std::string& name, GLenum format)
    {
        std::shared_ptr<OpenGLTexture2D> texture = std::make_shared<OpenGLTexture2D>(name, _width, _height, format, GL_CLAMP_TO_EDGE, GL_LINEAR);
        return addColorAttachment(texture, true);
    }

    void OpenGLFramebuffer::setDepthAttachment(const std::shared_ptr<OpenGLTexture2D>& texture, bool autoResize)
    {
        texture->bind();
        texture->generatePixelsBuffer();

        bind();
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->id(), 0));

        _depthAttachment = {texture, autoResize};
    }

    void OpenGLFramebuffer::createDepthAttachment(GLenum format)
    {
        std::shared_ptr<OpenGLTexture2D> texture = std::make_shared<OpenGLTexture2D>("Depth", _width, _height, format, GL_CLAMP_TO_EDGE, GL_LINEAR);
        setDepthAttachment(texture, true);
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
        for (const auto& attachment : _colorAttachments)
        {
            if (!attachment.autoResize)
            {
                continue;
            }
            
            attachment.texture->resize(width, height);
        }

        if (_depthAttachment.texture != nullptr && _depthAttachment.autoResize)
        {
            _depthAttachment.texture->resize(width, height);
        }
    }

    void OpenGLFramebuffer::bindAttachments()
    {
        for (std::size_t i = 0; i < _colorAttachments.size(); ++i)
        {
            _colorAttachments[i].texture->bind(static_cast<int>(i));
        }
    }
}
