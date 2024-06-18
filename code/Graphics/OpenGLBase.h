#pragma once

#include <Foundation/Base.h>
#include <Foundation/Log.h>

#include <glad/glad.h>

namespace BGLRenderer
{
    namespace Debug
    {
        static constexpr bool assertOpenGLCall = false;
    }

    extern Log openGLLogger;

    char const* openglErrorToString(const GLenum error) noexcept;
    void openglCheckError(const char* filename, size_t line);
}

#define GL_CALL(x) \
{ \
    x; \
    openglCheckError(__FILE__, __LINE__); \
}
