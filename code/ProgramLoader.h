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

        bool tryToUpdateVertexShader(const std::shared_ptr<OpenGLProgram>& program, const std::string& shaderName);
        bool tryToUpdateFragmentShader(const std::shared_ptr<OpenGLProgram>& program, const std::string& shaderName);

    private:
        std::shared_ptr<AssetContentLoader> _contentLoader;
    };

}
