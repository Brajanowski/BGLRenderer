#pragma once

#pragma warning(push)
#pragma warning(disable : 4996)
#pragma warning(disable : 5054)
#include <rapidjson/document.h>
#pragma warning(pop)

#include <string>
#include "GLMMath.h"
#include "Log.h"

namespace BGLRenderer
{
    namespace RapidJSONParsers
    {
        extern Log logger;
    }

    bool checkDocumentForAssetType(const rapidjson::Document& document, const std::string& requiredAssetType);

    template <typename TVal>
    TVal getMemberValue(const rapidjson::Value& parent, const std::string& memberName)
    {
        namespace RJP = RapidJSONParsers;

        if (!parent.IsObject())
        {
            RJP::logger.error("Failed at getting member value, parent is not an object");
            return {};
        }

        if (!parent.HasMember(memberName.c_str()))
        {
            RJP::logger.error("Failed at getting member value, requested member \"{}\" doesn't exists", memberName);
            return {};
        }

        const rapidjson::Value& value = parent[memberName.c_str()];

        if constexpr (std::is_same_v<TVal, std::string>)
        {
            if (!value.IsString())
            {
                RJP::logger.error("Failed at getting member value, member \"{}\" is not a string", memberName);
                return {};
            }

            return std::string(value.GetString());
        }
        else if constexpr (std::is_same_v<TVal, glm::vec2>)
        {
            if (!value.IsObject())
            {
                RJP::logger.error("Failed at getting member value, member \"{}\" must be an object with x and y member float values", memberName);
                return {};
            }

            if (!value.HasMember("x") || !value.HasMember("y"))
            {
                RJP::logger.error("Failed at getting member value, member \"{}\" must be an object with x and y member float values", memberName);
                return {};
            }

            return glm::vec2(value["x"].GetFloat(), value["y"].GetFloat());
        }
        else if constexpr (std::is_same_v<TVal, glm::vec3>)
        {
            if (!value.IsObject())
            {
                RJP::logger.error("Failed at getting member value, member \"{}\" must be an object with x, y and z member float values", memberName);
                return {};
            }

            if (!value.HasMember("x") || !value.HasMember("y") || !value.HasMember("z"))
            {
                RJP::logger.error("Failed at getting member value, member \"{}\" must be an object with x, y and z member float values", memberName);
                return {};
            }

            return glm::vec3(value["x"].GetFloat(), value["y"].GetFloat(), value["z"].GetFloat());
        }
        else if constexpr (std::is_same_v<TVal, glm::vec4>)
        {
            if (!value.IsObject())
            {
                RJP::logger.error("Failed at getting member value, member \"{}\" must be an object with x, y, z and w member float values", memberName);
                return {};
            }

            if (!value.HasMember("x") || !value.HasMember("y") || !value.HasMember("z") || !value.HasMember("w"))
            {
                RJP::logger.error("Failed at getting member value, member \"{}\" must be an object with x, y, z and w member float values", memberName);
                return {};
            }

            return glm::vec4(value["x"].GetFloat(), value["y"].GetFloat(), value["z"].GetFloat(), value["w"].GetFloat());
        }

        static_assert("Not implemented");
    }

    bool getStringValuesFromArray(const rapidjson::Value& documentValue, std::vector<std::string>& target);
}
