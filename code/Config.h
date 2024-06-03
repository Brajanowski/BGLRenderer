#pragma once

#include "RapidJSONParsers.h"

namespace BGLRenderer
{
    enum class ConfigValueType
    {
        invalid,
        string,
        floatValue,
        intValue,
        vector2,
        vector3,
        vector4
    };

    class Config
    {
    public:
        Config();

        float getFloat(const std::string& key);
        int getInt(const std::string& key);
        glm::vec2 getVector2(const std::string& key);
        glm::vec3 getVector3(const std::string& key);
        glm::vec4 getVector4(const std::string& key);
        std::string getString(const std::string& key);

        ConfigValueType getEntryValueType(const std::string& key) const;

        inline bool exists(const std::string& key) const { return getEntryValueType(key) != ConfigValueType::invalid; }

        void loadValuesFromDocument(const rapidjson::Document& document);

    private:
        static Log _logger;

        struct ConfigValue
        {
            ConfigValueType type;

            union
            {
                float floatValue;
                int intValue;
                glm::vec2 vector2;
                glm::vec3 vector3;
                glm::vec4 vector4;
            };

            std::string string{};
        };

        std::unordered_map<std::string, ConfigValue> _values;
    };
}
