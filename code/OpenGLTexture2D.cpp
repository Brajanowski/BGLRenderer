#include "OpenGLTexture2D.h"

namespace BGLRenderer
{
    OpenGLTexture2D::OpenGLTexture2D(GLuint width, GLuint height, GLenum format, GLenum wrapMode, GLenum filterMode) :
        _width(width),
        _height(height),
        _format(format),
        _wrapMode(wrapMode),
        _filterMode(filterMode)
    {
        GL_CALL(glGenTextures(1, &_id));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, _id));

        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _wrapMode));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _wrapMode));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _filterMode));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _filterMode));
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        GL_CALL(glDeleteTextures(1, &_id));
    }

    void OpenGLTexture2D::setPixels(GLuint format, GLbyte* pixels)
    {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, _id));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, _format, _width, _height, 0, format, GL_BYTE, pixels));
    }

    void OpenGLTexture2D::setPixels(GLuint format, GLfloat* pixels)
    {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, _id));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, _format, _width, _height, 0, format, GL_FLOAT, pixels));
    }

    void OpenGLTexture2D::bind(int slot)
    {
        GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, _id));
    }

    void OpenGLTexture2D::setWrapMode(GLenum wrapMode)
    {
        _wrapMode = wrapMode;

        bind(0);
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _wrapMode));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _wrapMode));
    }

    void OpenGLTexture2D::setFilterMode(GLenum filterMode)
    {
        _filterMode = filterMode;

        bind(0);
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _filterMode));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _filterMode));
    }
}
