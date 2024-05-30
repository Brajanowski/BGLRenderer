#pragma once

#include "AssetContentLoader.h"
#include "ConcreteAssetManager.h"

#include "Log.h"

namespace BGLRenderer
{
    class MaterialLoader
    {
    public:
        MaterialLoader(const std::shared_ptr<AssetContentLoader>& contentLoader,
                       const std::shared_ptr<TextureAssetManager>& textureAssetManager,
                       const std::shared_ptr<ProgramAssetManager>& programAssetManager);

        std::shared_ptr<OpenGLMaterial> load(const std::string& name);
        bool tryToUpdateMaterial(const std::shared_ptr<OpenGLMaterial>& material, const std::string& name);

    private:
        Log _logger{"Material Loader"};

        std::shared_ptr<AssetContentLoader> _contentLoader;
        std::shared_ptr<TextureAssetManager> _textureAssetManager;
        std::shared_ptr<ProgramAssetManager> _programAssetManager;
    };
}
