#pragma once

#include <memory>

#include "AssetContentLoader.h"
#include <Foundation/Config.h>

namespace BGLRenderer
{
    class ConfigLoader
    {
    public:
        ConfigLoader(const std::shared_ptr<AssetContentLoader>& contentLoader);

        std::shared_ptr<Config> loadJSON(const std::string& name);

    private:
        std::shared_ptr<AssetContentLoader> _contentLoader;
    };
}
