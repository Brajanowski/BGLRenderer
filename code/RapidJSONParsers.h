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

    bool isMemberVector2(const rapidjson::Value& member);
    bool isMemberVector3(const rapidjson::Value& member);
    bool isMemberVector4(const rapidjson::Value& member);

    template <typename TVal>
    TVal getMemberValue(const rapidjson::Value& member)
    {
        namespace RJP = RapidJSONParsers;

        if constexpr (std::is_same_v<TVal, std::string>)
        {
            if (!member.IsString())
            {
                RJP::logger.error("Failed at getting member value, member is not a string");
                return {};
            }

            return std::string(member.GetString());
        }
        else if constexpr (std::is_same_v<TVal, glm::vec2>)
        {
            if (!member.IsObject())
            {
                RJP::logger.error("Failed at getting member value, member must be an object with x and y member float values");
                return {};
            }

            if (!isMemberVector2(member))
            {
                RJP::logger.error("Failed at getting member value, member  must be an object with x and y member float values");
                return {};
            }

            return glm::vec2(member["x"].GetFloat(), member["y"].GetFloat());
        }
        else if constexpr (std::is_same_v<TVal, glm::vec3>)
        {
            if (!member.IsObject())
            {
                RJP::logger.error("Failed at getting member value, member must be an object with x, y and z member float values");
                return {};
            }

            if (!isMemberVector3(member))
            {
                RJP::logger.error("Failed at getting member value, member must be an object with x, y and z member float values");
                RJP::logger.error("{} count", member.MemberCount());
                return {};
            }

            return glm::vec3(member["x"].GetFloat(), member["y"].GetFloat(), member["z"].GetFloat());
        }
        else if constexpr (std::is_same_v<TVal, glm::vec4>)
        {
            if (!member.IsObject())
            {
                RJP::logger.error("Failed at getting member value, member must be an object with x, y, z and w member float values");
                return {};
            }

            if (!isMemberVector4(member))
            {
                RJP::logger.error("Failed at getting member value, member must be an object with x, y, z and w member float values");
                return {};
            }

            return glm::vec4(member["x"].GetFloat(), member["y"].GetFloat(), member["z"].GetFloat(), member["w"].GetFloat());
        }

        static_assert("Not implemented");
    }

    bool getStringValuesFromArray(const rapidjson::Value& documentValue, std::vector<std::string>& target);
}
