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
        float32,
        vector2,
        vector3,
        vector4,
        matrix4x4,
        texture,
        invalid
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

    enum class MaterialType
    {
        Opaque = 0,
        Transparent = 1,
        Unlit = 2
    };

    class OpenGLMaterial
    {
    public:
        OpenGLMaterial(const std::string& name, MaterialType type, const std::shared_ptr<OpenGLProgram>& program);
        OpenGLMaterial(const std::string& name, MaterialType type);
        ~OpenGLMaterial();

        void bind();

        void setFloat(const std::string& name, float value);
        void setVector2(const std::string& name, const glm::vec2& value);
        void setVector3(const std::string& name, const glm::vec3& value);
        void setVector4(const std::string& name, const glm::vec4& value);
        void setMatrix4x4(const std::string& name, const glm::mat4x4& value);
        void setTexture2D(const std::string& name, const std::shared_ptr<OpenGLTexture2D>& texture);

        inline void resetValues() { _valuesMap.clear(); }

        inline const std::shared_ptr<OpenGLProgram>& program() const { return _program; }

        void setProgram(const std::shared_ptr<OpenGLProgram>& program);

        inline const std::string& name() const { return _name; }
        inline std::string& name() { return _name; }

        inline MaterialType type() { return _type; }
        inline void changeType(MaterialType type) { _type = type; }

        inline bool valid() const { return _program != nullptr && !_program->hasErrors(); }

    private:
        std::string _name;
        MaterialType _type;

        std::shared_ptr<OpenGLProgram> _program;
        PublisherEmpty::ListenerHandle _programLinkedListenerHandle = PublisherEmpty::listenerHandleInvalid;
        
        std::map<std::string, OpenGLMaterialValue> _valuesMap;

        OpenGLMaterialValue* getOrCreateValue(const std::string& name);

        void programDidLinked();
    };
}
