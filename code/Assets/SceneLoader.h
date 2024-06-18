#pragma once

#include "AssetContentLoader.h"
#include "ConcreteAssetManager.h"

#include <Foundation/Log.h>
#include <Utility/RapidJSONParsers.h>
#include <World/Scene.h>

namespace BGLRenderer
{
    class AssetManager;

    class SceneLoader
    {
    public:
        SceneLoader(const std::shared_ptr<AssetContentLoader>& assetContentLoader,
                    const std::shared_ptr<ModelAssetManager>& modelAssetManager,
                    const std::shared_ptr<MaterialAssetManager>& materialAssetManager,
                    const std::shared_ptr<ProgramAssetManager>& programAssetManager);
        ~SceneLoader();

        std::shared_ptr<Scene> load(const std::string& name);
        bool loadInto(const std::shared_ptr<Scene>& scene, const std::string& name);

    private:
        Log _logger{"SceneLoader"};

        std::shared_ptr<AssetContentLoader> _assetContentLoader;
        std::shared_ptr<ModelAssetManager> _modelAssetManager;
        std::shared_ptr<MaterialAssetManager> _materialAssetManager;
        std::shared_ptr<ProgramAssetManager> _programAssetManager;

        bool loadSceneObject(const rapidjson::Value& entry, const std::shared_ptr<Scene>& toScene);
    };
}
