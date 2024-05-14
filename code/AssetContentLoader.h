#pragma once

#include "Base.h"

#include <string>
#include <vector>

namespace BGLRenderer
{
    class AssetContentLoader
    {
    public:
        std::vector<std::uint8_t> load(const std::string& path);

    private:
        std::string _assetsFolderPath = "./assets/";
    };
}
