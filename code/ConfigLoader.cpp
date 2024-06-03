#include "ConfigLoader.h"

namespace BGLRenderer
{
    ConfigLoader::ConfigLoader(const std::shared_ptr<AssetContentLoader>& contentLoader) :
        _contentLoader(contentLoader)
    {
    }

    std::shared_ptr<Config> ConfigLoader::loadJSON(const std::string& name)
    {
        std::vector<std::uint8_t> sceneAssetContent = _contentLoader->load(name);
        std::string assetContentString = std::string(sceneAssetContent.begin(), sceneAssetContent.end());

        rapidjson::Document document;
        document.Parse(assetContentString.c_str());

        std::shared_ptr<Config> config = std::make_shared<Config>();
        config->loadValuesFromDocument(document);

        return config;
    }
}
