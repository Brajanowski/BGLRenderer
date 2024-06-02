#include "OpenGLProgram.h"

#include <vector>

#include <gtc/type_ptr.hpp>

namespace BGLRenderer
{
    OpenGLProgram::OpenGLProgram(const std::string& name, const std::string& vertexShaderCode, const std::string& fragmentShaderCode) :
        _name(name),
        _vertexShaderCode(vertexShaderCode),
        _fragmentShaderCode(fragmentShaderCode)
    {
        openGLLogger.debug("Creating OpenGL program \"{}\":", name);

        _program = glCreateProgram();
        ASSERT(_program != 0, "Failed to create opengl program");

        //openGLLogger.debug("Vertex shader code:\n{}", vertexShaderCode);
        _vertexShader = createShader(vertexShaderCode, GL_VERTEX_SHADER);

        if (_vertexShader == 0)
        {
            openGLLogger.error("Failed to compile vertex shader for program \"{}\"", name);
            _hasErrors = true;
            return;
        }

        GL_CALL(glAttachShader(_program, _vertexShader));

        //openGLLogger.debug("Fragment shader code:\n{}", fragmentShaderCode);
        _fragmentShader = createShader(fragmentShaderCode, GL_FRAGMENT_SHADER);

        if (_fragmentShader == 0)
        {
            openGLLogger.error("Failed to compile fragment shader for program \"{}\"", name);
            _hasErrors = true;
            return;
        }

        GL_CALL(glAttachShader(_program, _fragmentShader));

        link();
    }

    OpenGLProgram::~OpenGLProgram()
    {
        if (_vertexShader != 0)
        {
            GL_CALL(glDeleteShader(_vertexShader));
        }

        if (_fragmentShader != 0)
        {
            GL_CALL(glDeleteShader(_fragmentShader));
        }

        GL_CALL(glDeleteProgram(_program));
    }

    bool OpenGLProgram::tryToUpdateVertexShader(const std::string& shaderCode)
    {
        bind();

        GLuint oldShader = _vertexShader;
        GL_CALL(glDetachShader(_program, oldShader));

        _vertexShader = createShader(shaderCode, GL_VERTEX_SHADER);

        if (_vertexShader == 0)
        {
            openGLLogger.error("Failed to update vertex shader!");
            _vertexShader = oldShader;
            return false;
        }

        GL_CALL(glAttachShader(_program, _vertexShader));

        if (!link())
        {
            GL_CALL(glDetachShader(_program, _vertexShader));
            GL_CALL(glDeleteShader(_vertexShader));
            _vertexShader = oldShader;

            GL_CALL(glAttachShader(_program, _fragmentShader));

            link();

            return false;
        }

        GL_CALL(glDeleteShader(oldShader));

        _vertexShaderCode = shaderCode;
        return true;
    }

    bool OpenGLProgram::tryToUpdateFragmentShader(const std::string& shaderCode)
    {
        bind();

        GLuint oldShader = _fragmentShader;
        GL_CALL(glDetachShader(_program, oldShader));

        _fragmentShader = createShader(shaderCode, GL_FRAGMENT_SHADER);

        if (_fragmentShader == 0)
        {
            openGLLogger.error("Failed to update fragment shader!");
            _fragmentShader = oldShader;
            return false;
        }

        GL_CALL(glAttachShader(_program, _fragmentShader));

        if (!link())
        {
            GL_CALL(glDetachShader(_program, _fragmentShader));
            GL_CALL(glDeleteShader(_fragmentShader));
            _fragmentShader = oldShader;

            GL_CALL(glAttachShader(_program, _fragmentShader));

            link();

            return false;
        }

        GL_CALL(glDeleteShader(oldShader));

        _fragmentShaderCode = shaderCode;
        return true;
    }

    void OpenGLProgram::bind()
    {
        GL_CALL(glUseProgram(_program));
    }

    void OpenGLProgram::setInt(GLint location, GLint value)
    {
        glUniform1i(location, value);
    }

    void OpenGLProgram::setFloat(GLint location, GLfloat value)
    {
        glUniform1f(location, value);
    }

    void OpenGLProgram::setVector2(GLint location, const glm::vec2& value)
    {
        glUniform2f(location, value.x, value.y);
    }

    void OpenGLProgram::setVector3(GLint location, const glm::vec3& value)
    {
        glUniform3f(location, value.x, value.y, value.z);
    }

    void OpenGLProgram::setVector4(GLint location, const glm::vec4& value)
    {
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    void OpenGLProgram::setMatrix4x4(GLint location, const glm::mat4x4& value)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, const_cast<float*>(glm::value_ptr(value)));
    }

    GLint OpenGLProgram::getUniformLocation(const std::string& name)
    {
        GLint location = glGetUniformLocation(_program, name.c_str());
        return location;
    }

    bool OpenGLProgram::link()
    {
        openGLLogger.debug("Linking program...");

        GL_CALL(glLinkProgram(_program));

        GLint isLinked = GL_FALSE;
        GL_CALL(glGetProgramiv(_program, GL_LINK_STATUS, &isLinked));

        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            GL_CALL(glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &maxLength));

            std::vector<GLchar> infoLog(maxLength);
            GL_CALL(glGetProgramInfoLog(_program, maxLength, &maxLength, infoLog.data()));

            openGLLogger.debug("Program \"{}\" linking error:\n{}", _name, infoLog.data());

            _hasErrors = true;
            return false;
        }
        _hasErrors = false;

        openGLLogger.debug("Program \"{}\" successfully linked!", _name);

        programLinkedPublisher().publish();
        return true;
    }

    GLuint OpenGLProgram::createShader(const std::string& code, GLuint shaderType)
    {
        GLuint shader = glCreateShader(shaderType);

        const char* sourceCString = code.c_str();
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

            openGLLogger.debug("Shader compilation error:\n{}", infoLog.data());
            glDeleteShader(shader);
            return 0;
        }

        openGLLogger.debug("Shader successfully compiled!");

        return shader;
    }
}
