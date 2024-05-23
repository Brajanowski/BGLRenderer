#pragma once

#include "Base.h"

#include <vector>
#include <filesystem>

namespace BGLRenderer
{
    class AssetContentLoader
    {
    public:
        bool fileExists(const std::filesystem::path& path);
        
        std::vector<std::uint8_t> load(const std::filesystem::path& path);

        std::filesystem::file_time_type getLastWriteTime(const std::filesystem::path& path);

    private:
        std::filesystem::path _assetsFolderPath = "./assets/";

        std::filesystem::path getAssetPath(const std::filesystem::path& path);
    };
}
