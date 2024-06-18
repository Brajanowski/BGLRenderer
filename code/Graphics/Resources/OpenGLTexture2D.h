#pragma once

#include "../OpenGLBase.h"

namespace BGLRenderer
{
    enum class FilterMode
    {
        pixel,
        linear
    };

    enum class WrapMode
    {
        repeat,
        clampToEdge
    };
    
    class OpenGLTexture2D
    {
    public:
        OpenGLTexture2D(const std::string& name, GLuint width, GLuint height, GLenum format, WrapMode wrapMode = WrapMode::repeat, FilterMode filterMode = FilterMode::linear);
        ~OpenGLTexture2D();

        void resize(GLuint width, GLuint height);

        void setPixels(GLuint format, GLbyte *pixels);
        void setPixels(GLuint format, GLubyte *pixels);
        void setPixels(GLuint format, GLfloat *pixels);

        void generatePixelsBuffer();

        void bind(int slot = 0);
        void unbind();

        void setWrapMode(WrapMode wrapMode);
        void setFilterMode(FilterMode filterMode);

        void generateMipmaps(int max = -1);

        inline GLuint id() { return _id; }

        inline const std::string& name() const { return _name; }

        static GLenum getDefaultPixelDataFormatFor(GLenum pixelFormat);

    private:
        std::string _name;
        GLuint _id;
        GLuint _width;
        GLuint _height;
        GLenum _format;
        WrapMode _wrapMode;
        FilterMode _filterMode;

        bool _hasMipMaps = false;

        int _bindSlot = 0;

        void updateTextureParametersNoBinding();
    };
}
