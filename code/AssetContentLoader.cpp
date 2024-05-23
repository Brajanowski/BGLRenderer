#include "AssetContentLoader.h"

namespace BGLRenderer
{
    bool AssetContentLoader::fileExists(const std::filesystem::path& path)
    {
        std::filesystem::path filePath = getAssetPath(path);
        return std::filesystem::exists(filePath);
    }

    std::vector<std::uint8_t> AssetContentLoader::load(const std::filesystem::path& path)
    {
        std::filesystem::path filePath = getAssetPath(path);

        FILE* file = fopen(filePath.string().c_str(), "rb");
        fseek(file, 0, SEEK_END);
        std::size_t size = ftell(file);
        fseek(file, 0, SEEK_SET);

        std::vector<std::uint8_t> content;
        content.resize(size, 0);

        fread(content.data(), size, 1, file);
        fclose(file);
        return content;
    }

    std::filesystem::file_time_type AssetContentLoader::getLastWriteTime(const std::filesystem::path& path)
    {
        std::filesystem::path filePath = getAssetPath(path);
        return std::filesystem::last_write_time(filePath);
    }

    std::filesystem::path AssetContentLoader::getAssetPath(const std::filesystem::path& path)
    {
        return _assetsFolderPath / path;
    }
}
