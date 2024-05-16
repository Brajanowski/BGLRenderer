#pragma once

#include <string>

#include "OpenGLBase.h"

#include <glm.hpp>

namespace BGLRenderer
{
    class OpenGLProgram
    {
    public:
        OpenGLProgram(const std::string& vertexShaderCode, const std::string& fragmentShaderCode);
        ~OpenGLProgram();

        void bind();

        void setFloat(GLint location, float value);
        void setVector2(GLint location, const glm::vec2& value);
        void setVector3(GLint location, const glm::vec3& value);
        void setVector4(GLint location, const glm::vec4& value);
        void setMatrix4x4(GLint location, const glm::mat4x4& value);

        GLint getUniformLocation(const std::string& name);
    private:
        GLuint _program;
        GLuint _fragmentShader;
        GLuint _vertexShader;

        void link();

        GLuint createShader(const std::string &code, GLuint shaderType);
    };
}
