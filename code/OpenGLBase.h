#pragma once

#include "Base.h"

#include <glad/glad.h>

#include "Log.h"

static BGLRenderer::Log openGLLogger = BGLRenderer::Log("OpenGL");

static char const* openglErrorToString(const GLenum error) noexcept
{
    switch (error)
    {
    // opengl 2 errors (8)
    case GL_NO_ERROR:
        return "GL_NO_ERROR";

    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";

    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";

    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";

    case GL_STACK_OVERFLOW:
        return "GL_STACK_OVERFLOW";

    case GL_STACK_UNDERFLOW:
        return "GL_STACK_UNDERFLOW";

    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";

    case GL_TABLE_TOO_LARGE:
        return "GL_TABLE_TOO_LARGE";

    // opengl 3 errors (1)
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";

    // gles 2, 3 and gl 4 error are handled by the switch above
    default:
        ASSERT(false, "unknown opengl error");
        return nullptr;
    }
}

static void openglCheckError(const char* filename, size_t line)
{
    GLenum error = glGetError();

    if (error != GL_NO_ERROR)
    {
        openGLLogger.error("{}: {}: {}", filename, line, openglErrorToString(error));
    }
}

#define GL_CALL(_X) \
{ \
    _X; \
    openglCheckError(__FILE__, __LINE__); \
}
