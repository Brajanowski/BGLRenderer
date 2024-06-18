#pragma once

#include "../OpenGLBase.h"

namespace BGLRenderer
{
    enum class OpenGLCubeFace : GLenum
    {
        positiveX = 0,
        negativeX = 1,
        positiveY = 2,
        negativeY = 3,
        positiveZ = 4,
        negativeZ = 5
    };

    class OpenGLCubemap
    {
    public:
        OpenGLCubemap(const std::string& name, GLuint faceSize, GLenum format);
        ~OpenGLCubemap();

        void bind(int slot);

        inline const std::string& name() const { return _name; }
        inline GLuint faceSize() const { return _faceSize; }
        inline GLenum format() const { return _format; }

        inline GLuint id() { return _id; }

    private:
        std::string _name;
        GLuint _id;
        GLuint _faceSize;
        GLenum _format;
    };
}
