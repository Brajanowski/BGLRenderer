#include "OpenGLProgram.h"

#include <iostream>
#include <vector>

#include <gtc/type_ptr.hpp>

namespace BGLRenderer
{
    OpenGLProgram::OpenGLProgram(const std::string& vertexShaderCode, const std::string& fragmentShaderCode)
    {
        std::cout << "Creating OpenGL Program:" << std::endl;

        _program = glCreateProgram();
        ASSERT(_program != 0, "failed to create opengl program");

        std::cout << "Vertex shader code: " << std::endl;
        std::cout << vertexShaderCode << std::endl;
        _vertexShader = createShader(vertexShaderCode, GL_VERTEX_SHADER);
        ASSERT(_vertexShader != 0, "failed to compile vertex shader");

        std::cout << "Fragment shader code: " << std::endl;
        std::cout << fragmentShaderCode << std::endl;
        _fragmentShader = createShader(fragmentShaderCode, GL_FRAGMENT_SHADER);
        ASSERT(_fragmentShader != 0, "failed to compile fragment shader");

        link();
    }

    OpenGLProgram::~OpenGLProgram()
    {
        GL_CALL(glDeleteProgram(_program));
    }

    void OpenGLProgram::bind()
    {
        GL_CALL(glUseProgram(_program));
    }

    void OpenGLProgram::setInt(GLint location, int value)
    {
        GL_CALL(glUniform1i(location, value));
    }

    void OpenGLProgram::setFloat(GLint location, float value)
    {
        GL_CALL(glUniform1f(location, value));
    }

    void OpenGLProgram::setVector2(GLint location, const glm::vec2& value)
    {
        GL_CALL(glUniform2f(location, value.x, value.y));
    }

    void OpenGLProgram::setVector3(GLint location, const glm::vec3& value)
    {
        GL_CALL(glUniform3f(location, value.x, value.y, value.z));
    }

    void OpenGLProgram::setVector4(GLint location, const glm::vec4& value)
    {
        GL_CALL(glUniform4f(location, value.x, value.y, value.z, value.w));
    }

    void OpenGLProgram::setMatrix4x4(GLint location, const glm::mat4x4& value)
    {
        GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, const_cast<float*>(glm::value_ptr(value))));
    }

    GLint OpenGLProgram::getUniformLocation(const std::string& name)
    {
        GLint location = glGetUniformLocation(_program, name.c_str());

        if (location == -1)
        {
            std::cout << "Couldn't find uniform location: " << name << std::endl;
        }

        ASSERT(location != -1, "Failed to find uniform location");
        return location;
    }

    void OpenGLProgram::link()
    {
        std::cout << "Linking program..." << std::endl;

        GL_CALL(glAttachShader(_program, _vertexShader));
        GL_CALL(glAttachShader(_program, _fragmentShader));

        GL_CALL(glLinkProgram(_program));

        GLint isLinked = GL_FALSE;
        GL_CALL(glGetProgramiv(_program, GL_LINK_STATUS, &isLinked));

        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            GL_CALL(glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &maxLength));

            std::vector<GLchar> infoLog(maxLength);
            GL_CALL(glGetProgramInfoLog(_program, maxLength, &maxLength, infoLog.data()));

            std::cout << "shader linking error:" << std::endl << infoLog.data() << std::endl;

            ASSERT(false, "error occured while linking program");
        }
    }

    GLuint OpenGLProgram::createShader(const std::string& code, GLuint shaderType)
    {
        GLuint shader = glCreateShader(shaderType);

        const char *sourceCString = code.c_str();
        GLint sourceLength = (GLint)code.length();

        GL_CALL(glShaderSource(shader, 1, &sourceCString, &sourceLength));
        GL_CALL(glCompileShader(shader));

        GLint isCompiled = 0;
        GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled));

        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            GL_CALL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength));

            std::vector<GLchar> infoLog(maxLength);
            GL_CALL(glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog.data()));

            std::cout << "Shader compilation error: " << std::endl << infoLog.data() << std::endl;
            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }
}
