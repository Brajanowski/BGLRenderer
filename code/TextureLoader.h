#pragma once

#include "OpenGLTexture2D.h"
#include "AssetContentLoader.h"

namespace BGLRenderer
{
    class TextureLoader
    {
    public:
        TextureLoader(const std::shared_ptr<AssetContentLoader>& contentLoader);
        ~TextureLoader() = default;
        
        std::shared_ptr<OpenGLTexture2D> loadTexture2D(const std::string& name);

        std::shared_ptr<OpenGLTexture2D> loadTextureFromImageData(const std::vector<std::uint8_t>& imageFileContent);
        std::shared_ptr<OpenGLTexture2D> loadTextureFromImageData(const std::uint8_t* bytes, size_t size);
    private:
        std::shared_ptr<AssetContentLoader> _contentLoader;
    };
}
