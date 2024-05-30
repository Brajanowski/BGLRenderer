#include "MaterialLoader.h"

#pragma warning(push)
#pragma warning(disable : 4996)
#pragma warning(disable : 5054)
#include <rapidjson/document.h>
#pragma warning(pop)

namespace BGLRenderer
{
    constexpr const char* FallbackMaterialName = "fallback";

    static OpenGLMaterialValueType materialValueTypeIntToEnum(int value)
    {
        switch (value)
        {
        case 0:
            return OpenGLMaterialValueType::texture;
        case 1:
            return OpenGLMaterialValueType::float32;
        case 2:
            return OpenGLMaterialValueType::vector2;
        case 3:
            return OpenGLMaterialValueType::vector3;
        case 4:
            return OpenGLMaterialValueType::vector4;
        case 5:
            return OpenGLMaterialValueType::matrix4x4;
        default:
            return OpenGLMaterialValueType::invalid;
        }
    }

    bool parseMaterialValue(const std::shared_ptr<OpenGLMaterial>& material,
                            const std::string& valueName,
                            rapidjson::Value& documentValue,
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
        std::vector<std::uint8_t> materialContent = _contentLoader->load(name);
        std::string materialContentString = std::string(materialContent.begin(), materialContent.end());

        rapidjson::Document document;
        document.Parse(materialContentString.c_str());

        std::shared_ptr<OpenGLMaterial> material = std::make_shared<OpenGLMaterial>(name, MaterialType::Opaque);

        if (!document.IsObject())
        {
            _logger.error("Document is not an object");
            return material;
        }

        const rapidjson::Value& programValue = document["program"];

        if (programValue.IsNull())
        {
            _logger.error("Couldn't find 'program' value");
            return material;
        }

        if (!programValue.HasMember("vertexShader"))
        {
            _logger.error("Couldn't find 'vertexShader' inside of 'program'");
            return material;
        }
        const rapidjson::Value& vertexShaderName = programValue["vertexShader"];

        if (!programValue.HasMember("fragmentShader"))
        {
            _logger.error("Couldn't find 'fragmentShader' inside of 'program'");
            return material;
        }
        const rapidjson::Value& fragmentShaderName = programValue["fragmentShader"];

        // TODO: check if values are the correct types

        std::shared_ptr<OpenGLProgram> program = _programAssetManager->get({vertexShaderName.GetString(), fragmentShaderName.GetString()});
        material->setProgram(program);

        for (auto& member : document.GetObject())
        {
            std::string memberName(member.name.GetString());
            if (memberName == "program")
            {
                continue;
            }

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

        const rapidjson::Value& programValue = document["program"];

        if (programValue.IsNull())
        {
            _logger.error("Couldn't find 'program' value");
            return false;
        }

        if (!programValue.HasMember("vertexShader"))
        {
            _logger.error("Couldn't find 'vertexShader' inside of 'program'");
            return false;
        }
        const rapidjson::Value& vertexShaderName = programValue["vertexShader"];

        if (!programValue.HasMember("fragmentShader"))
        {
            _logger.error("Couldn't find 'fragmentShader' inside of 'program'");
            return false;
        }
        const rapidjson::Value& fragmentShaderName = programValue["fragmentShader"];

        std::shared_ptr<OpenGLProgram> program = _programAssetManager->get({vertexShaderName.GetString(), fragmentShaderName.GetString()});

        if (program == nullptr)
        {
            _logger.error("Couldn't load program!");
            return false;
        }

        material->setProgram(program);

        for (auto& member : document.GetObject())
        {
            std::string memberName(member.name.GetString());
            if (memberName == "program")
            {
                continue;
            }

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
        rapidjson::Value& documentValue,
        Log& logger,
        const std::shared_ptr<TextureAssetManager>& textureAssetManager)
    {
        if (!documentValue.HasMember("value_type"))
        {
            logger.warning("Skipping member \"{}\" because it doesn't \"value_type\" member", valueName);
            return false;
        }

        if (!documentValue["value_type"].IsInt())
        {
            logger.warning("Skipping document's member \"{}\" because \"value_type\" is not an int", valueName);
            return false;
        }

        OpenGLMaterialValueType valueType = materialValueTypeIntToEnum(documentValue["value_type"].GetInt());

        if (valueType == OpenGLMaterialValueType::invalid)
        {
            logger.warning("Skipping document's member \"{}\" because it has invalid \"value_type\"", valueName);
            return false;
        }

        if (!documentValue.HasMember("value"))
        {
            logger.warning("Skipping document's member \"{}\" because it doesn't has \"value\" member", valueName);
            return false;
        }

        rapidjson::Value& valueObject = documentValue["value"];

        if (!valueObject.IsObject())
        {
            logger.warning("Skipping document's member \"{}\" because \"value\" is not an object", valueName);
            return false;
        }

        if (valueType == OpenGLMaterialValueType::texture)
        {
            if (!valueObject.HasMember("texture"))
            {
                logger.warning("Texture entry doesn't have texture inside it's \"value\" object");
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
        else if (valueType == OpenGLMaterialValueType::float32)
        {
            if (!valueObject.HasMember("value"))
            {
                logger.warning("Float entry doesn't have \"value\" field inside it's \"value\" object");
                return false;
            }

            float floatValue = valueObject["value"].GetFloat();
            material->setFloat(valueName, floatValue);
        }
        else if (valueType == OpenGLMaterialValueType::vector2)
        {
            if (!valueObject.HasMember("x") ||
                !valueObject.HasMember("y"))
            {
                logger.warning("Vector2 entry doesn't have every component values! Skipping");
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
                !valueObject.HasMember("z"))
            {
                logger.warning("Vector3 entry doesn't have every component values! Skipping");
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
                !valueObject.HasMember("w"))
            {
                logger.warning("Vector4 entry doesn't have every component values! Skipping");
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
            logger.warning("Skipping member \"{}\" because it doesn't \"has value_type\" member", valueName);
            return false;
        }

        return true;
    }
}
