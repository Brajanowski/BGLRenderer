#include "RapidJSONParsers.h"

#include "Log.h"

namespace BGLRenderer
{
    namespace RapidJSONParsers
    {
        Log logger{"RapidJSONParsers"};
    }

    using namespace RapidJSONParsers;

    namespace MemberNames
    {
        namespace Private
        {
            using NameValueType = const char *;
        }

        static constexpr Private::NameValueType AssetType = "asset_type";
    }

    bool checkDocumentForAssetType(const rapidjson::Document& document, const std::string& requiredAssetType)
    {
        if (!document.IsObject())
        {
            logger.error("Document is not an object");
            return false;
        }

        if (!document.HasMember(MemberNames::AssetType))
        {
            logger.error("Couldn't find \"{}\" member object", MemberNames::AssetType);
            return false;
        }

        const rapidjson::Value& value = document[MemberNames::AssetType];

        if (!value.IsString())
        {
            logger.error("\"{}\" must be a string type", MemberNames::AssetType);
            return false;
        }

        bool is = requiredAssetType == value.GetString();

        if (!is)
        {
            logger.error("\"{}\" is not \"{}\"", MemberNames::AssetType, requiredAssetType);
        }

        return is; 
    }

    bool getStringValuesFromArray(const rapidjson::Value& documentValue, std::vector<std::string>& target)
    {
        if (!documentValue.IsArray())
        {
            logger.error("Failed at getting string values from array. Document value is not an array!");
            return false;
        }

        target.clear();
        target.reserve(documentValue.Size());

        for (rapidjson::SizeType i = 0; i < documentValue.Size(); ++i)
        {
            if (!documentValue[i].IsString())
            {
                logger.error("Failed at getting string values from array, Array entry is not a string, failed at {}", i);
                continue;
            }

            target.push_back(std::string(documentValue[i].GetString()));
        }

        return true;
    }
}
