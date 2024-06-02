#pragma once

#include "Scene.h"
#include "AssetContentLoader.h"
#include "ConcreteAssetManager.h"
#include "Log.h"

#include "RapidJSONParsers.h"

namespace BGLRenderer
{
    class AssetManager;

    class SceneLoader
    {
    public:
        SceneLoader(const std::shared_ptr<AssetContentLoader>& assetContentLoader,
                    const std::shared_ptr<ModelAssetManager>& modelAssetManager,
                    const std::shared_ptr<MaterialAssetManager>& materialAssetManager);
        ~SceneLoader();

        std::shared_ptr<Scene> load(const std::string& name);
        bool loadInto(const std::shared_ptr<Scene>& scene, const std::string& name);

    private:
        Log _logger{"SceneLoader"};

        std::shared_ptr<AssetContentLoader> _assetContentLoader;
        std::shared_ptr<ModelAssetManager> _modelAssetManager;
        std::shared_ptr<MaterialAssetManager> _materialAssetManager;

        bool loadSceneObject(const rapidjson::Value& entry, const std::shared_ptr<Scene>& toScene);
    };
}
