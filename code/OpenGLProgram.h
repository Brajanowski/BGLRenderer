#pragma once

#include <string>

#include "GLMMath.h"

#include "OpenGLBase.h"
#include "Publisher.h"

namespace BGLRenderer
{
    class OpenGLProgram
    {
    public:
        OpenGLProgram(const std::string& name, const std::string& vertexShaderCode, const std::string& fragmentShaderCode);
        ~OpenGLProgram();

        bool tryToUpdateVertexShader(const std::string& shaderCode);
        bool tryToUpdateFragmentShader(const std::string& shaderCode);

        void bind();

        void setInt(GLint location, GLint value);
        void setFloat(GLint location, GLfloat value);
        void setVector2(GLint location, const glm::vec2& value);
        void setVector3(GLint location, const glm::vec3& value);
        void setVector4(GLint location, const glm::vec4& value);
        void setMatrix4x4(GLint location, const glm::mat4x4& value);

        GLint getUniformLocation(const std::string& name);

        inline const std::string& name() const { return _name; }
        inline PublisherEmpty& programLinkedPublisher() { return _programLinkedPublisher; }

        inline bool hasErrors() const { return _hasErrors; }

        inline const std::string& vertexShaderCode() const { return _vertexShaderCode; }
        inline const std::string& fragmentShaderCode() const { return _fragmentShaderCode; }

    private:
        std::string _name;
        GLuint _program;
        GLuint _fragmentShader;
        GLuint _vertexShader;

        std::string _vertexShaderCode;
        std::string _fragmentShaderCode;

        PublisherEmpty _programLinkedPublisher{};

        bool _hasErrors = true;

        bool link();

        GLuint createShader(const std::string& code, GLuint shaderType);
    };
}
