#include "TextureLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace BGLRenderer
{
    TextureLoader::TextureLoader(const std::shared_ptr<AssetContentLoader>& contentLoader) :
        _contentLoader(contentLoader)
    {
    }

    std::shared_ptr<OpenGLTexture2D> TextureLoader::loadTexture2D(const std::string& name)
    {
        std::vector<std::uint8_t> textureFileContent = _contentLoader->load(name);
        std::shared_ptr<OpenGLTexture2D> texture = loadTextureFromImageData(textureFileContent);
        return texture;
    }

    std::shared_ptr<OpenGLTexture2D> TextureLoader::loadTextureFromImageData(const std::vector<std::uint8_t>& imageFileContent)
    {
        return loadTextureFromImageData(imageFileContent.data(), imageFileContent.size());
    }

    std::shared_ptr<OpenGLTexture2D> TextureLoader::loadTextureFromImageData(const std::uint8_t* bytes, size_t size)
    {
        ASSERT(bytes != nullptr, "Bytes is nullptr");
        ASSERT(size > 0, "Invalid size");

        int width;
        int height;
        int components;
        float* imageData = stbi_loadf_from_memory(bytes, static_cast<int>(size), &width, &height, &components, 0);
        ASSERT(imageData != nullptr, "Failed to load image data");

        GLenum internalFormat;
        GLenum dataFormat = GL_RGBA;

        bool gammaCorrection = false;

        if (components == 1)
        {
            internalFormat = dataFormat = GL_RED;
        }
        else if (components == 3)
        {
            internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (components == 4)
        {
            internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }
        else
        {
            ASSERT(false, "Invalid number of components must be in range of 1-4");
        }

        std::shared_ptr<OpenGLTexture2D> texture = std::make_shared<OpenGLTexture2D>(std::format("Texture2D_{}x{}x{}", width, height, components), width, height, internalFormat);
        texture->setPixels(dataFormat, (GLfloat*)imageData);

        stbi_image_free(imageData);
        return texture;
    }
}
