#include "OpenGLTexture2D.h"

namespace BGLRenderer
{
    OpenGLTexture2D::OpenGLTexture2D(const std::string& name, GLuint width, GLuint height, GLenum format,
                                     WrapMode wrapMode, FilterMode filterMode) :
        _name(name),
        _width(width),
        _height(height),
        _format(format),
        _wrapMode(wrapMode),
        _filterMode(filterMode)
    {
        GL_CALL(glGenTextures(1, &_id));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, _id));
        updateTextureParametersNoBinding();
        generatePixelsBuffer();
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        GL_CALL(glDeleteTextures(1, &_id));
    }

    void OpenGLTexture2D::resize(GLuint width, GLuint height)
    {
        openGLLogger.debug("Resizing texture \"{}\" to {}x{}", _name, width, height);

        _width = width;
        _height = height;

        generatePixelsBuffer();
    }

    void OpenGLTexture2D::setPixels(GLuint format, GLbyte* pixels)
    {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, _id));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, _format, _width, _height, 0, format, GL_BYTE, pixels));
    }

    void OpenGLTexture2D::setPixels(GLuint format, GLubyte* pixels)
    {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, _id));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, _format, _width, _height, 0, format, GL_UNSIGNED_BYTE, pixels));
    }

    void OpenGLTexture2D::setPixels(GLuint format, GLfloat* pixels)
    {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, _id));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, _format, _width, _height, 0, format, GL_FLOAT, pixels));
    }

    void OpenGLTexture2D::generatePixelsBuffer()
    {
        bind();
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, _format,
            _width, _height, 0,
            getDefaultPixelDataFormatFor(_format),
            GL_UNSIGNED_BYTE, nullptr));
    }

    void OpenGLTexture2D::bind(int slot)
    {
        _bindSlot = slot;

        GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, _id));
    }

    void OpenGLTexture2D::unbind()
    {
        GL_CALL(glActiveTexture(GL_TEXTURE0 + _bindSlot));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }

    void OpenGLTexture2D::setWrapMode(WrapMode wrapMode)
    {
        _wrapMode = wrapMode;

        bind(0);
        updateTextureParametersNoBinding();
    }

    void OpenGLTexture2D::setFilterMode(FilterMode filterMode)
    {
        _filterMode = filterMode;

        bind(0);
        updateTextureParametersNoBinding();
    }

    void OpenGLTexture2D::generateMipmaps(int max)
    {
        _hasMipMaps = true;

        bind(0);

        if (max > 0)
        {
            GL_CALL(glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, max));
        }

        GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

        updateTextureParametersNoBinding();
    }

    void OpenGLTexture2D::updateTextureParametersNoBinding()
    {
        GLenum wrapMode = GL_REPEAT;

        if (_wrapMode == WrapMode::repeat)
        {
            wrapMode = GL_REPEAT;
        }
        else if (_wrapMode == WrapMode::clampToEdge)
        {
            wrapMode = GL_CLAMP_TO_EDGE;
        }

        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode));

        GLenum minFilterMode = GL_LINEAR;
        GLenum magFilterMode = GL_LINEAR;

        if (_filterMode == FilterMode::pixel)
        {
            if (_hasMipMaps)
            {
                minFilterMode = GL_NEAREST_MIPMAP_NEAREST;
                magFilterMode = GL_NEAREST;
            }
            else
            {
                minFilterMode = GL_NEAREST;
                magFilterMode = GL_NEAREST;
            }
        }
        else if (_filterMode == FilterMode::linear)
        {
            if (_hasMipMaps)
            {
                minFilterMode = GL_LINEAR_MIPMAP_LINEAR;
                magFilterMode = GL_LINEAR;
            }
            else
            {
                minFilterMode = GL_LINEAR;
                magFilterMode = GL_LINEAR;
            }
        }

        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilterMode));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterMode));
    }

    /// @brief For given texture internal format returns data format that is valid for given format
    /// e.g. GL_RGBA32F -> GL_RGBA
    /// TODO - more entries needs to be added
    /// NOTE - but on the other hand I feel like this solution might be terrible and should be done in completly
    ///        different way but for now this solution will do
    GLenum OpenGLTexture2D::getDefaultPixelDataFormatFor(GLenum pixelFormat)
    {
        static const std::unordered_map<GLenum, GLenum> lookup = {
            {GL_RGBA8, GL_RGBA},
            {GL_RGB8, GL_RGB},
            {GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT},
            {GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT},
            {GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL},
            {GL_R8, GL_RED},
            {GL_RG8, GL_RG},
            {GL_R32F, GL_RED},
            {GL_RG32F, GL_RG},
            {GL_RGBA32F, GL_RGBA},
            {GL_RGB32F, GL_RGB},
            {GL_RED, GL_RED},
            {GL_RGB, GL_RGB},
            {GL_RGBA, GL_RGBA}
        };

        auto it = lookup.find(pixelFormat);
        ASSERT(it != lookup.end(), "Couldn't find entry for given pixel format");
        return it->second;
    }
}
