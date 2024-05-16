# include "OpenGLMaterial.h"

namespace BGLRenderer
{
    OpenGLMaterial::OpenGLMaterial(const std::shared_ptr<OpenGLProgram>& program) :
        _program(program)
    {
    }

    OpenGLMaterial::~OpenGLMaterial()
    {
    }

    void OpenGLMaterial::bind()
    {
        _program->bind();

        int textureSlot = 0;

        for (const auto& [key, value] : _valuesMap)
        {
            switch (value.type)
            {
            case OpenGLMaterialValueType::Float:
                _program->setFloat(value.uniformLocation, value.floatValue);
                break;
            case OpenGLMaterialValueType::Vector2:
                _program->setVector2(value.uniformLocation, value.vec2);
                break;
            case OpenGLMaterialValueType::Vector3:
                _program->setVector3(value.uniformLocation, value.vec3);
                break;
            case OpenGLMaterialValueType::Vector4:
                _program->setVector4(value.uniformLocation, value.vec4);
                break;
            case OpenGLMaterialValueType::Matrix4x4:
                _program->setMatrix4x4(value.uniformLocation, value.mat4x4);
                break;
            case OpenGLMaterialValueType::Texture:
                value.texture->bind(textureSlot);
                _program->setInt(value.uniformLocation, textureSlot);

                textureSlot++;
                break;
            }
        }
    }

    void OpenGLMaterial::setFloat(const std::string& name, float value)
    {
        OpenGLMaterialValue* materialValue = getOrCreateValue(name);
        materialValue->type = OpenGLMaterialValueType::Float;
        materialValue->floatValue = value;
    }

    void OpenGLMaterial::setVector2(const std::string& name, const glm::vec2& value)
    {
        OpenGLMaterialValue* materialValue = getOrCreateValue(name);
        materialValue->type = OpenGLMaterialValueType::Vector2;
        materialValue->vec2 = value;
    }

    void OpenGLMaterial::setVector3(const std::string& name, const glm::vec3& value)
    {
        OpenGLMaterialValue* materialValue = getOrCreateValue(name);
        materialValue->type = OpenGLMaterialValueType::Vector3;
        materialValue->vec3 = value;
    }

    void OpenGLMaterial::setVector4(const std::string& name, const glm::vec4& value)
    {
        OpenGLMaterialValue* materialValue = getOrCreateValue(name);
        materialValue->type = OpenGLMaterialValueType::Vector4;
        materialValue->vec4 = value;
    }

    void OpenGLMaterial::setMatrix4x4(const std::string& name, const glm::mat4x4& value)
    {
        OpenGLMaterialValue* materialValue = getOrCreateValue(name);
        materialValue->type = OpenGLMaterialValueType::Matrix4x4;
        materialValue->mat4x4 = value;
    }

    void OpenGLMaterial::setTexture2D(const std::string& name, const std::shared_ptr<OpenGLTexture2D>& texture)
    {
        OpenGLMaterialValue* materialValue = getOrCreateValue(name);
        materialValue->type = OpenGLMaterialValueType::Texture;
        materialValue->texture = texture;
    }

    OpenGLMaterialValue* OpenGLMaterial::getOrCreateValue(const std::string& name)
    {
        if (!_valuesMap.contains(name))
        {
            OpenGLMaterialValue value{};
            value.name = name;
            value.uniformLocation = _program->getUniformLocation(name);
            _valuesMap[name] = value;
        }

        return &_valuesMap[name];
    }
}
