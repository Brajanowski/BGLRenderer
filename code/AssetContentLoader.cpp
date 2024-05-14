#include "AssetContentLoader.h"

namespace BGLRenderer
{
    std::vector<std::uint8_t> AssetContentLoader::load(const std::string& path)
    {
        FILE* file = fopen((_assetsFolderPath + path).c_str(), "rb");
        fseek(file, 0, SEEK_END);
        std::size_t size = ftell(file);
        fseek(file, 0, SEEK_SET);

        std::vector<std::uint8_t> content;
        content.resize(size, 0);

        fread(content.data(), size, 1, file);
        fclose(file);
        return content;
    }
}
