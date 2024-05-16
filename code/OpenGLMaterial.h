#pragma once

#include <memory>
#include <string>
#include <map>

#include "OpenGLBase.h"
#include "OpenGLProgram.h"

#include <glm.hpp>

namespace BGLRenderer
{
    enum class OpenGLMaterialValueType
    {
        Float,
        Vector2,
        Vector3,
        Vector4,
        Matrix4x4
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
        void setMatrix4X4(const std::string& name, const glm::mat4x4& value);

    private:
        std::shared_ptr<OpenGLProgram> _program;
        std::map<std::string, OpenGLMaterialValue> _valuesMap;

        OpenGLMaterialValue* getOrCreateValue(const std::string& name);
    };
}
