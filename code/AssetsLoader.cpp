#include "AssetsLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace BGLRenderer
{
    AssetsLoader::AssetsLoader(const std::shared_ptr<AssetContentLoader>& contentLoader) :
        _contentLoader(contentLoader)
    {
        stbi_set_flip_vertically_on_load(true);
    }

    std::shared_ptr<OpenGLProgram> AssetsLoader::loadProgram(const std::string& name)
    {
        return loadProgram(name + ".vert", name + ".frag");
    }

    std::shared_ptr<OpenGLProgram> AssetsLoader::loadProgram(const std::string& vertexShaderName, const std::string& fragmentShaderName)
    {
        std::vector<std::uint8_t> vertexContent = _contentLoader->load(vertexShaderName);
        std::vector<std::uint8_t> fragmentContent = _contentLoader->load(fragmentShaderName);

        std::shared_ptr<OpenGLProgram> program = std::make_shared<OpenGLProgram>(std::string(vertexContent.begin(), vertexContent.end()), std::string(fragmentContent.begin(), fragmentContent.end()));
        return program;
    }

    std::shared_ptr<OpenGLTexture2D> AssetsLoader::loadTexture(const std::string& name)
    {
        std::vector<std::uint8_t> textureFileContent = _contentLoader->load(name);

        int width;
        int height;
        int components;
        float* imageData = stbi_loadf_from_memory(textureFileContent.data(), textureFileContent.size(), &width, &height, &components, 0);
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

        std::shared_ptr<OpenGLTexture2D> texture = std::make_shared<OpenGLTexture2D>(width, height, internalFormat);
        texture->setPixels(dataFormat, (GLfloat*)imageData);

        stbi_image_free(imageData);

        return texture;
    }
}
