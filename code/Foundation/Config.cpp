#include "Config.h"

namespace BGLRenderer
{
    Log Config::_logger = {"Config"};

    Config::Config()
    {
    }

    float Config::getFloat(const std::string& key)
    {
        if (!_values.contains(key))
        {
            _logger.error("\"{}\" value doesn't exists");
            return 0.0f;
        }

        const ConfigValue& configValue = _values[key];

        if (configValue.type != ConfigValueType::floatValue)
        {
            _logger.error("\"{}\" is not a float");
            return 0.0f;
        }

        return configValue.floatValue;
    }

    int Config::getInt(const std::string& key)
    {
        if (!_values.contains(key))
        {
            _logger.error("\"{}\" value doesn't exists");
            return 0;
        }

        const ConfigValue& configValue = _values[key];

        if (configValue.type != ConfigValueType::intValue)
        {
            _logger.error("\"{}\" is not a int");
            return 0;
        }

        return configValue.intValue;
    }

    glm::vec2 Config::getVector2(const std::string& key)
    {
        if (!_values.contains(key))
        {
            _logger.error("\"{}\" value doesn't exists");
            return {};
        }

        const ConfigValue& configValue = _values[key];

        if (configValue.type != ConfigValueType::vector2)
        {
            _logger.error("\"{}\" is not a vector2");
            return {};
        }

        return configValue.vector2;
    }

    glm::vec3 Config::getVector3(const std::string& key)
    {
        if (!_values.contains(key))
        {
            _logger.error("\"{}\" value doesn't exists");
            return {};
        }

        const ConfigValue& configValue = _values[key];

        if (configValue.type != ConfigValueType::vector3)
        {
            _logger.error("\"{}\" is not a vector3");
            return {};
        }

        return configValue.vector3;
    }

    glm::vec4 Config::getVector4(const std::string& key)
    {
        if (!_values.contains(key))
        {
            _logger.error("\"{}\" value doesn't exists");
            return {};
        }

        const ConfigValue& configValue = _values[key];

        if (configValue.type != ConfigValueType::vector4)
        {
            _logger.error("\"{}\" is not a vector4");
            return {};
        }

        return configValue.vector4;
    }

    std::string Config::getString(const std::string& key)
    {
        if (!_values.contains(key))
        {
            _logger.error("\"{}\" value doesn't exists");
            return {};
        }

        const ConfigValue& configValue = _values[key];

        if (configValue.type != ConfigValueType::string)
        {
            _logger.error("\"{}\" is not a string");
            return {};
        }

        return configValue.string;
    }

    ConfigValueType Config::getEntryValueType(const std::string& key) const
    {
        if (!_values.contains(key))
        {
            return ConfigValueType::invalid;
        }

        return _values.at(key).type;
    }

    void Config::loadValuesFromDocument(const rapidjson::Document& document)
    {
        if (!document.IsObject())
        {
            _logger.error("Document is not an object");
            return;
        }

        for (auto it = document.MemberBegin(); it != document.MemberEnd(); ++it)
        {
            std::string name = it->name.GetString();

            if (_values.contains(name))
            {
                _logger.warning("Value \"{}\" already exists", name);
                continue;
            }
            
            const rapidjson::Value& value = it->value;

            ConfigValue configValue{};

            if (value.IsInt())
            {
                configValue.type = ConfigValueType::intValue;
                configValue.intValue = value.GetInt();
            }
            else if (value.IsFloat())
            {
                configValue.type = ConfigValueType::floatValue;
                configValue.floatValue = value.GetFloat();
            }
            else if (value.IsString())
            {
                configValue.type = ConfigValueType::string;
                configValue.string = value.GetString();
            }
            else if (value.IsObject())
            {
                if (isMemberVector2(value))
                {
                    configValue.type = ConfigValueType::vector2;
                    configValue.vector2 = getMemberValue<glm::vec2>(value);
                }
                else if (isMemberVector3(value))
                {
                    configValue.type = ConfigValueType::vector3;
                    configValue.vector3 = getMemberValue<glm::vec3>(value);
                }
                else if (isMemberVector4(value))
                {
                    configValue.type = ConfigValueType::vector4;
                    configValue.vector3 = getMemberValue<glm::vec4>(value);
                }
            }
            else
            {
                _logger.error("Couldn't parse value \"{}\"", name);
                continue;
            }

            _values[name] = configValue;
        }
    }
}
