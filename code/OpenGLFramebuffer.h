#pragma once

#include "OpenGLBase.h"
#include "OpenGLTexture2D.h"

namespace BGLRenderer
{
    struct OpenGLFramebufferAttachmentDetails
    {
        std::shared_ptr<OpenGLTexture2D> texture;
        bool autoResize = true;
    };

    class OpenGLFramebuffer
    {
    public:
        OpenGLFramebuffer(const std::string& name, GLuint width, GLuint height);
        ~OpenGLFramebuffer();

        int addColorAttachment(const std::shared_ptr<OpenGLTexture2D>& texture, bool autoResize = false);

        /// @brief Creates color attachment for this framebuffer, returns attachment index
        int createColorAttachment(const std::string& name, GLenum format);

        void setDepthAttachment(const std::shared_ptr<OpenGLTexture2D>& texture, bool autoResize = false);
        void createDepthAttachment(GLenum format);

        bool validate();

        void resize(GLuint width, GLuint height);

        void bind();
        void unbind();

        void bindAttachments();

        inline const std::string& name() const { return _name; }

        inline GLuint id() { return _id; }
        inline const std::vector<OpenGLFramebufferAttachmentDetails>& colorAttachments() const { return _colorAttachments; }
        inline const OpenGLFramebufferAttachmentDetails& depthAttachment() const { return _depthAttachment; }

    private:
        std::string _name;
        GLuint _id;
        GLuint _width;
        GLuint _height;

        std::vector<OpenGLFramebufferAttachmentDetails> _colorAttachments;
        OpenGLFramebufferAttachmentDetails _depthAttachment{};
    };
}
