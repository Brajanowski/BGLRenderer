#pragma once

#include "OpenGLBase.h"
#include "OpenGLTexture2D.h"

namespace BGLRenderer
{
    class OpenGLFramebuffer
    {
    public:
        OpenGLFramebuffer(GLuint width, GLuint height);
        ~OpenGLFramebuffer();

        /// @brief Creates color attachment for this framebuffer, returns attachment index
        int createColorAttachment(GLenum format);

        void createDepthAttachment(GLenum format);

        bool validate();

        void resize(GLuint width, GLuint height);

        void bind();
        void unbind();

        void bindAttachments();

        inline GLuint id() { return _id; }
        inline const std::vector<std::shared_ptr<OpenGLTexture2D>>& colorAttachments() const { return _colorAttachments; }
        inline const std::shared_ptr<OpenGLTexture2D>& depthAttachment() const { return _depthAttachment; }

    private:
        GLuint _id;
        GLuint _width;
        GLuint _height;
        std::vector<std::shared_ptr<OpenGLTexture2D>> _colorAttachments;
        std::shared_ptr<OpenGLTexture2D> _depthAttachment = nullptr;
    };
}
