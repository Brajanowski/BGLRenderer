#pragma once

#include "OpenGLProgram.h"
#include "AssetContentLoader.h"

namespace BGLRenderer
{
    class ProgramLoader
    {
    public:
        ProgramLoader(const std::shared_ptr<AssetContentLoader>& contentLoader);
        ~ProgramLoader() = default;

        std::shared_ptr<OpenGLProgram> loadProgram(const std::string& vertexShaderName, const std::string& fragmentShaderName);

    private:
        std::shared_ptr<AssetContentLoader> _contentLoader;
    };

}
