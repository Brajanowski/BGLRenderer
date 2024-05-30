#include "MaterialLoader.h"

#pragma warning(push)
#pragma warning(disable : 4996)
#pragma warning(disable : 5054)
#include <rapidjson/document.h>
#pragma warning(pop)

namespace BGLRenderer
{
    namespace MaterialSettings
    {
        static constexpr std::string_view program = "program";
        static constexpr std::string_view programVertexShader = "vertexShader";
        static constexpr std::string_view programFragmentShader = "fragmentShader";
        static constexpr std::string_view type = "type";
        static constexpr std::string_view values = "values";

        static constexpr std::string_view valueObjectTypeMember = "value_type";
        static constexpr std::string_view valueObjectValueMember = "value";

        static constexpr MaterialType defaultMaterialType = MaterialType::opaque;
    }

    /// @brief translates string view to 'MaterialType' enum, if fails to parse string return 'defaultMaterialType' instead
    static constexpr MaterialType materialTypeFromString(const std::string_view& str)
    {
        if (str == "opaque")
        {
            return MaterialType::opaque;
        }

        if (str == "transparent")
        {
            return MaterialType::transparent;
        }

        if (str == "unlit")
        {
            return MaterialType::unlit;
        }

        return MaterialSettings::defaultMaterialType;
    }


    /// @brief translates string view to 'OpenGLMaterialValueType' enum, if fails to parse string returns invalid
    static constexpr OpenGLMaterialValueType materialValueTypeFromString(const std::string_view& str)
    {
        if (str == "int")
        {
            return OpenGLMaterialValueType::int32;
        }

        if (str == "float")
        {
            return OpenGLMaterialValueType::float32;
        }

        if (str == "vec2")
        {
            return OpenGLMaterialValueType::vector2;
        }

        if (str == "vec3")
        {
            return OpenGLMaterialValueType::vector3;
        }

        if (str == "vec4")
        {
            return OpenGLMaterialValueType::vector4;
        }

        if (str == "texture2D")
        {
            return OpenGLMaterialValueType::texture;
        }

        if (str == "mat4")
        {
            return OpenGLMaterialValueType::matrix4x4;
        }

        return OpenGLMaterialValueType::invalid;
    }

    bool parseMaterialValue(const std::shared_ptr<OpenGLMaterial>& material,
                            const std::string& valueName,
                            const rapidjson::Value& documentValue,
                            Log& logger,
                            const std::shared_ptr<TextureAssetManager>& textureAssetManager);

    MaterialLoader::MaterialLoader(const std::shared_ptr<AssetContentLoader>& contentLoader,
                                   const std::shared_ptr<TextureAssetManager>& textureAssetManager,
                                   const std::shared_ptr<ProgramAssetManager>& programAssetManager) :
        _contentLoader(contentLoader),
        _textureAssetManager(textureAssetManager),
        _programAssetManager(programAssetManager)
    {
    }

    std::shared_ptr<OpenGLMaterial> MaterialLoader::load(const std::string& name)
    {
        std::shared_ptr<OpenGLMaterial> material = std::make_shared<OpenGLMaterial>(name, MaterialType::opaque);

        if (!tryToUpdateMaterial(material, name))
        {
            _logger.error("Couldn't parse material: \"{}\"", name);
        }

        return material;
    }

    bool MaterialLoader::tryToUpdateMaterial(const std::shared_ptr<OpenGLMaterial>& material, const std::string& name)
    {
        ASSERT(material != nullptr, "Cannot update null material");

        std::vector<std::uint8_t> materialContent = _contentLoader->load(name);
        std::string materialContentString = std::string(materialContent.begin(), materialContent.end());

        rapidjson::Document document;
        document.Parse(materialContentString.c_str());

        if (!document.IsObject())
        {
            _logger.error("Document is not an object");
            return false;
        }

        /// MARK - get program
        const rapidjson::Value& programObject = document[MaterialSettings::program.data()];

        if (programObject.IsNull())
        {
            _logger.error("Couldn't find '{}' value", MaterialSettings::program);
            return false;
        }

        if (!programObject.HasMember(MaterialSettings::programVertexShader.data()))
        {
            _logger.error("Couldn't find '{}' inside of '{}'", MaterialSettings::programVertexShader, MaterialSettings::program);
            return false;
        }
        const rapidjson::Value& vertexShaderName = programObject[MaterialSettings::programVertexShader.data()];

        if (!programObject.HasMember(MaterialSettings::programFragmentShader.data()))
        {
            _logger.error("Couldn't find '{}' inside of '{}'", MaterialSettings::programFragmentShader, MaterialSettings::program);
            return false;
        }
        const rapidjson::Value& fragmentShaderName = programObject[MaterialSettings::programFragmentShader.data()];

        std::shared_ptr<OpenGLProgram> program = _programAssetManager->get({vertexShaderName.GetString(), fragmentShaderName.GetString()});

        if (program == nullptr)
        {
            _logger.error("Couldn't load program!");
            return false;
        }

        material->setProgram(program);

        /// MARK - get material type
        if (document.HasMember(MaterialSettings::type.data()))
        {
            const rapidjson::Value& typeValue = document[MaterialSettings::type.data()];

            if (typeValue.IsString())
            {
                std::string_view sv = typeValue.GetString();

                MaterialType type = materialTypeFromString(sv);
                material->changeType(type);
            }
        }

        /// MARK - read values
        if (!document.HasMember(MaterialSettings::values.data()))
        {
            _logger.debug("Loaded material without any values because root object '{}', doesn't exist", MaterialSettings::values.data());
            return true;
        }

        const rapidjson::Value& valuesObject = document[MaterialSettings::values.data()];

        /// MARK - set material values
        for (auto& member : valuesObject.GetObject())
        {
            std::string memberName(member.name.GetString());

            if (!member.value.IsObject())
            {
                _logger.warning("Skipping member \"{}\" because it's not an object", memberName);
                continue;
            }

            if (!parseMaterialValue(material, memberName, member.value, _logger, _textureAssetManager))
            {
                _logger.warning("Skipping material value \"{}\" because it couldn't be parsed", memberName);
            }
        }

        return true;
    }

    bool parseMaterialValue(const std::shared_ptr<OpenGLMaterial>& material,
                            const std::string& valueName,
                            const rapidjson::Value& documentValue,
                            Log& logger,
                            const std::shared_ptr<TextureAssetManager>& textureAssetManager)
    {
        if (!documentValue.HasMember(MaterialSettings::valueObjectTypeMember.data()))
        {
            logger.warning("Skipping member \"{}\" because it doesn't \"{}\" member", valueName, MaterialSettings::valueObjectTypeMember);
            return false;
        }

        if (!documentValue[MaterialSettings::valueObjectTypeMember.data()].IsString())
        {
            logger.warning("Skipping document's member \"{}\" because \"{}\" is not an int", valueName, MaterialSettings::valueObjectTypeMember);
            return false;
        }

        OpenGLMaterialValueType valueType = materialValueTypeFromString(documentValue[MaterialSettings::valueObjectTypeMember.data()].GetString());

        if (valueType == OpenGLMaterialValueType::invalid)
        {
            logger.warning("Skipping document's member \"{}\" because it has invalid value in member \"{}\"", valueName, MaterialSettings::valueObjectTypeMember);
            return false;
        }

        if (!documentValue.HasMember(MaterialSettings::valueObjectValueMember.data()))
        {
            logger.warning("Skipping document's member \"{}\" because it doesn't has \"{}\" member", valueName, MaterialSettings::valueObjectValueMember);
            return false;
        }

        const rapidjson::Value& valueObject = documentValue[MaterialSettings::valueObjectValueMember.data()];

        if (!valueObject.IsObject())
        {
            logger.warning("Skipping document's member \"{}\" because \"\" is not an object", valueName, MaterialSettings::valueObjectValueMember);
            return false;
        }

        if (valueType == OpenGLMaterialValueType::texture)
        {
            if (!valueObject.HasMember("texture"))
            {
                logger.warning("Texture entry doesn't have \"texture\" inside it's \"{}\" object", MaterialSettings::valueObjectValueMember);
                return false;
            }

            if (!valueObject["texture"].IsString())
            {
                logger.warning("\"texture\" is not a string");
                return false;
            }

            std::string textureName = valueObject["texture"].GetString();
            std::shared_ptr<OpenGLTexture2D> texture = textureAssetManager->get(textureName);

            if (texture == nullptr)
            {
                logger.error("Couldn't find texture \"{}\"", textureName);
                return false;
            }

            material->setTexture2D(valueName, texture);
        }
        else if (valueType == OpenGLMaterialValueType::int32)
        {
            if (!valueObject.HasMember("value"))
            {
                logger.error("Float entry doesn't have \"value\" in member \"{}\"", valueName);
                return false;
            }

            if (!valueObject["value"].IsInt())
            {
                logger.error("\"value\" is not an int in member \"{}\"", valueName);
                return false;
            }

            std::int32_t intValue = static_cast<std::int32_t>(valueObject["value"].GetInt());
            material->setInt(valueName, intValue);
        }
        else if (valueType == OpenGLMaterialValueType::float32)
        {
            if (!valueObject.HasMember("value"))
            {
                logger.error("Float entry doesn't have \"value\" in member \"{}\"", valueName);
                return false;
            }

            if (!valueObject["value"].IsFloat())
            {
                logger.error("\"value\" is not a float in member \"{}\"", valueName);
                return false;
            }

            std::float_t floatValue = static_cast<std::float_t>(valueObject["value"].GetFloat());
            material->setFloat(valueName, floatValue);
        }
        else if (valueType == OpenGLMaterialValueType::vector2)
        {
            if (!valueObject.HasMember("x") ||
                !valueObject.HasMember("y") ||
                !valueObject["x"].IsFloat() ||
                !valueObject["y"].IsFloat())
            {
                logger.error("Vector2 entry doesn't have every component values, remember that it requires 'x', 'y' members as float values in member \"{}\"! Skipping", valueName);
                return false;
            }

            glm::vec2 vec2Value = {
                valueObject["x"].GetFloat(),
                valueObject["y"].GetFloat()
            };

            material->setVector2(valueName, vec2Value);
        }
        else if (valueType == OpenGLMaterialValueType::vector3)
        {
            if (!valueObject.HasMember("x") ||
                !valueObject.HasMember("y") ||
                !valueObject.HasMember("z") ||
                !valueObject["x"].IsFloat() ||
                !valueObject["y"].IsFloat() ||
                !valueObject["z"].IsFloat())
            {
                logger.warning("Vector3 entry doesn't have every component values, remember that it requires 'x', 'y', 'z' members as float values in member \"{}\"! Skipping", valueName);
                return false;
            }

            glm::vec3 vec3Value = {
                valueObject["x"].GetFloat(),
                valueObject["y"].GetFloat(),
                valueObject["z"].GetFloat()
            };

            material->setVector3(valueName, vec3Value);
        }

        else if (valueType == OpenGLMaterialValueType::vector4)
        {
            if (!valueObject.HasMember("x") ||
                !valueObject.HasMember("y") ||
                !valueObject.HasMember("z") ||
                !valueObject.HasMember("w") ||
                !valueObject["x"].IsFloat() ||
                !valueObject["y"].IsFloat() ||
                !valueObject["z"].IsFloat() ||
                !valueObject["w"].IsFloat())
            {
                logger.warning("Vector4 entry doesn't have every component values, remember that it requires 'x', 'y', 'z', 'w' members as float values in member \"{}\"! Skipping", valueName);
                return false;
            }

            glm::vec4 vec4Value = {
                valueObject["x"].GetFloat(),
                valueObject["y"].GetFloat(),
                valueObject["z"].GetFloat(),
                valueObject["w"].GetFloat()
            };

            material->setVector4(valueName, vec4Value);
        }
        else
        {
            logger.warning("Skipping member \"{}\" because value type is not supported: \"{}\"", valueName, MaterialSettings::valueObjectTypeMember);
            return false;
        }

        return true;
    }
}
