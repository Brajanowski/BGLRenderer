#pragma once

#include "OpenGLBase.h"
#include "AssetContentLoader.h"
#include "OpenGLProgram.h"
#include "OpenGLTexture2D.h"

namespace BGLRenderer
{
    class AssetsLoader
    {
    public:
        AssetsLoader(const std::shared_ptr<AssetContentLoader>& contentLoader);

        std::shared_ptr<OpenGLProgram> loadProgram(const std::string& name);
        std::shared_ptr<OpenGLProgram> loadProgram(const std::string& vertexShaderName, const std::string& fragmentShaderName);

        std::shared_ptr<OpenGLTexture2D> loadTexture(const std::string& name);

    private:
        std::shared_ptr<AssetContentLoader> _contentLoader;
    };
}
