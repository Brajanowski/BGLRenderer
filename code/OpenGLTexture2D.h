#pragma once

#include "OpenGLBase.h"

namespace BGLRenderer
{
    class OpenGLTexture2D
    {
    public:
        OpenGLTexture2D(GLuint width, GLuint height, GLenum format, GLenum wrapMode = GL_CLAMP_TO_EDGE, GLenum filterMode = GL_LINEAR);
        ~OpenGLTexture2D();

        void setPixels(GLuint format, GLbyte *pixels);
        void setPixels(GLuint format, GLfloat *pixels);

        void bind(int slot = 0);

        void setWrapMode(GLenum wrapMode);
        void setFilterMode(GLenum filterMode);

    private:
        GLuint _id;
        GLuint _width;
        GLuint _height;
        GLenum _format;
        GLenum _wrapMode;
        GLenum _filterMode;
    };
}
