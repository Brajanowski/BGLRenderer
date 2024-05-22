﻿#pragma once

#include "OpenGLBase.h"

namespace BGLRenderer
{
    class OpenGLTexture2D
    {
    public:
        OpenGLTexture2D(GLuint width, GLuint height, GLenum format, GLenum wrapMode = GL_CLAMP_TO_EDGE, GLenum filterMode = GL_LINEAR);
        ~OpenGLTexture2D();

        void resize(GLuint width, GLuint height);

        void setPixels(GLuint format, GLbyte *pixels);
        void setPixels(GLuint format, GLfloat *pixels);

        void generatePixelsBuffer();

        void bind(int slot = 0);
        void unbind();

        void setWrapMode(GLenum wrapMode);
        void setFilterMode(GLenum filterMode);

        inline GLuint id() { return _id; }

    private:
        GLuint _id;
        GLuint _width;
        GLuint _height;
        GLenum _format;
        GLenum _wrapMode;
        GLenum _filterMode;

        int _bindSlot = 0;
    };
}
