#pragma once

#include <string>

#include "OpenGLBase.h"

namespace BGLRenderer
{
    class OpenGLProgram
    {
    public:
        OpenGLProgram(const std::string& vertexShaderCode, const std::string& fragmentShaderCode);
        ~OpenGLProgram();

        void bind();

    private:
        GLuint _program;
        GLuint _fragmentShader;
        GLuint _vertexShader;

        void link();

        GLuint createShader(const std::string &code, GLuint shaderType);
    };
}
