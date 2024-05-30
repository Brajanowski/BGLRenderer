#include "AssetContentLoader.h"

#include <fstream>

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

        if (!std::filesystem::exists(filePath))
        {
            _logger.error("{} doesn't exists, returning empty buffer", filePath.string());
            return {};
        }

        if (!std::filesystem::is_regular_file(filePath))
        {
            _logger.error("{} is not a file, returning empty buffer", filePath.string());
            return {};
        }

        std::uintmax_t fileSize = std::filesystem::file_size(filePath);
        std::vector<std::uint8_t> buffer(fileSize, 0);

        std::ifstream is(filePath, std::ios::binary);

        if (!is.is_open())
        {
            _logger.error("Couldn't open file: {}, returning empty buffer", filePath.string());
            return {};
        }
        
        is.read(reinterpret_cast<char*>(buffer.data()), fileSize);

        if (is.gcount() == 0)
        {
            _logger.error("Failed to read content of the file: {}, returning empty buffer", filePath.string());
            return {};
        }

        return buffer;
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
