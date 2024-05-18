#pragma once

#include <memory>
#include <string>
#include <map>

#include "GLMMath.h"

#include "OpenGLBase.h"
#include "OpenGLProgram.h"
#include "OpenGLTexture2D.h"

namespace BGLRenderer
{
    enum class OpenGLMaterialValueType
    {
        Float,
        Vector2,
        Vector3,
        Vector4,
        Matrix4x4,
        Texture
    };

    struct OpenGLMaterialValue
    {
        OpenGLMaterialValueType type;
        std::string name;
        GLint uniformLocation;

        union
        {
            float floatValue;
            glm::vec2 vec2;
            glm::vec3 vec3;
            glm::vec4 vec4;
            glm::mat4x4 mat4x4;
        };

        std::shared_ptr<OpenGLTexture2D> texture;
    };
    
    class OpenGLMaterial
    {
    public:
        OpenGLMaterial(const std::shared_ptr<OpenGLProgram>& program);
        ~OpenGLMaterial();

        void bind();

        void setFloat(const std::string& name, float value);
        void setVector2(const std::string& name, const glm::vec2& value);
        void setVector3(const std::string& name, const glm::vec3& value);
        void setVector4(const std::string& name, const glm::vec4& value);
        void setMatrix4x4(const std::string& name, const glm::mat4x4& value);
        void setTexture2D(const std::string& name, const std::shared_ptr<OpenGLTexture2D> &texture);

    private:
        std::shared_ptr<OpenGLProgram> _program;
        std::map<std::string, OpenGLMaterialValue> _valuesMap;

        OpenGLMaterialValue* getOrCreateValue(const std::string& name);
    };
}
