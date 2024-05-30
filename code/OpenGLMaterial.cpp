#include "OpenGLMaterial.h"
#include "OpenGLProgram.h"

namespace BGLRenderer
{
    namespace Debug
    {
        constexpr bool LogMaterialValuesSetters = false;
    }

    OpenGLMaterial::OpenGLMaterial(const std::string& name, MaterialType type, const std::shared_ptr<OpenGLProgram>& program) :
        _name(name),
        _type(type),
        _program(program)
    {
        ASSERT(program != nullptr, "Program cannot be null when creating new material");
        _programLinkedListenerHandle = _program->programLinkedPublisher().listen([&] { programDidLinked(); });

        openGLLogger.debug("Creating new material \"{}\" using program \"{}\"", name, program->name());
    }

    OpenGLMaterial::OpenGLMaterial(const std::string& name, MaterialType type) :
        _name(name),
        _type(type),
        _program(nullptr)
    {
        openGLLogger.debug("Creating invalid material \"{}\" without the program", name);
    }

    OpenGLMaterial::~OpenGLMaterial()
    {
        openGLLogger.debug("Destroying material \"{}\"", _name);

        if (_programLinkedListenerHandle != PublisherEmpty::listenerHandleInvalid)
        {
            _program->programLinkedPublisher().removeListener(_programLinkedListenerHandle);
        }
    }

    void OpenGLMaterial::bind()
    {
        _program->bind();

        int textureSlot = 0;

        for (const auto& [key, value] : _valuesMap)
        {
            switch (value.type)
            {
            case OpenGLMaterialValueType::float32:
                _program->setFloat(value.uniformLocation, value.floatValue);
                break;
            case OpenGLMaterialValueType::vector2:
                _program->setVector2(value.uniformLocation, value.vec2);
                break;
            case OpenGLMaterialValueType::vector3:
                _program->setVector3(value.uniformLocation, value.vec3);
                break;
            case OpenGLMaterialValueType::vector4:
                _program->setVector4(value.uniformLocation, value.vec4);
                break;
            case OpenGLMaterialValueType::matrix4x4:
                _program->setMatrix4x4(value.uniformLocation, value.mat4x4);
                break;
            case OpenGLMaterialValueType::texture:
                value.texture->bind(textureSlot);
                _program->setInt(value.uniformLocation, textureSlot);

                textureSlot++;
                break;

            default:
                break;
            }
        }
    }

    void OpenGLMaterial::setFloat(const std::string& name, float value)
    {
        OpenGLMaterialValue* materialValue = getOrCreateValue(name);
        materialValue->type = OpenGLMaterialValueType::float32;
        materialValue->floatValue = value;

        if constexpr (Debug::LogMaterialValuesSetters)
        {
            openGLLogger.debug("Setting float {} as value \"{}\" in material \"{}\"",value, name, _name);
        }
    }

    void OpenGLMaterial::setVector2(const std::string& name, const glm::vec2& value)
    {
        OpenGLMaterialValue* materialValue = getOrCreateValue(name);
        materialValue->type = OpenGLMaterialValueType::vector2;
        materialValue->vec2 = value;

        if constexpr (Debug::LogMaterialValuesSetters)
        {
            openGLLogger.debug("Setting Vector2 {} as value \"{}\" in material \"{}\"", glm::to_string(value), name, _name);
        }
    }

    void OpenGLMaterial::setVector3(const std::string& name, const glm::vec3& value)
    {
        OpenGLMaterialValue* materialValue = getOrCreateValue(name);
        materialValue->type = OpenGLMaterialValueType::vector3;
        materialValue->vec3 = value;

        if constexpr (Debug::LogMaterialValuesSetters)
        {
            openGLLogger.debug("Setting Vector3 {} as value \"{}\" in material \"{}\"", glm::to_string(value), name, _name);
        }
    }

    void OpenGLMaterial::setVector4(const std::string& name, const glm::vec4& value)
    {
        OpenGLMaterialValue* materialValue = getOrCreateValue(name);
        materialValue->type = OpenGLMaterialValueType::vector4;
        materialValue->vec4 = value;

        if constexpr (Debug::LogMaterialValuesSetters)
        {
            openGLLogger.debug("Setting Vector4 {} as value \"{}\" in material \"{}\"", glm::to_string(value), name, _name);
        }
    }

    void OpenGLMaterial::setMatrix4x4(const std::string& name, const glm::mat4x4& value)
    {
        OpenGLMaterialValue* materialValue = getOrCreateValue(name);
        materialValue->type = OpenGLMaterialValueType::matrix4x4;
        materialValue->mat4x4 = value;

        if constexpr (Debug::LogMaterialValuesSetters)
        {
            openGLLogger.debug("Setting matrix \n{}\n as value \"{}\" in material \"{}\"", glm::to_string(value), name, _name);
        }
    }

    void OpenGLMaterial::setTexture2D(const std::string& name, const std::shared_ptr<OpenGLTexture2D>& texture)
    {
        OpenGLMaterialValue* materialValue = getOrCreateValue(name);
        materialValue->type = OpenGLMaterialValueType::texture;
        materialValue->texture = texture;

        if constexpr (Debug::LogMaterialValuesSetters)
        {
            openGLLogger.debug("Setting texture \"{}\" as value \"{}\" in material \"{}\"", texture->name(), name, _name);
        }
    }

    void OpenGLMaterial::setProgram(const std::shared_ptr<OpenGLProgram>& program)
    {
        ASSERT(program != nullptr, "Program cannot be nullptr!");

        openGLLogger.debug("Material \"{}\" is changing it's program, all values assigned to that material will reset", _name);
        resetValues();

        if (_program != nullptr)
        {
            _program->programLinkedPublisher().removeListener(_programLinkedListenerHandle);
        }

        _program = program;

        _programLinkedListenerHandle = _program->programLinkedPublisher().listen([&] { programDidLinked(); });
    }

    OpenGLMaterialValue* OpenGLMaterial::getOrCreateValue(const std::string& name)
    {
        if (!_valuesMap.contains(name))
        {
            OpenGLMaterialValue value{};
            value.name = name;
            value.uniformLocation = _program->getUniformLocation("u_" + name);
            _valuesMap[name] = value;
        }

        return &_valuesMap[name];
    }

    void OpenGLMaterial::programDidLinked()
    {
        for (auto& [key, val] : _valuesMap)
        {
            std::string uniformName = "u_" + val.name;
            val.uniformLocation = _program->getUniformLocation(uniformName);

            if (val.uniformLocation == -1)
            {
                openGLLogger.warning("Material \"{}\": couldn't find uniform \"{}\" inside of new program.", _name, uniformName);
            }
        }
    }
}
