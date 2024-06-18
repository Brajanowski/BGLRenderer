#include "OpenGLCubemap.h"

#include "OpenGLTexture2D.h"

namespace BGLRenderer
{
    OpenGLCubemap::OpenGLCubemap(const std::string &name, GLuint faceSize, GLenum format) :
        _name(name),
        _faceSize(faceSize),
        _format(format)
    {
        GL_CALL(glGenTextures(1, &_id));
        GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, _id));

        GLenum dataFormat = OpenGLTexture2D::getDefaultPixelDataFormatFor(format);

        for (GLuint i = 0; i < 6; ++i)
        {
            GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         format,
                         _faceSize, _faceSize,
                         0,
                         dataFormat,
                         GL_FLOAT, nullptr));
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    OpenGLCubemap::~OpenGLCubemap()
    {
        GL_CALL(glDeleteTextures(1, &_id));
    }

    void OpenGLCubemap::bind(int slot)
    {
        GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
        GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, _id));
    }
}
