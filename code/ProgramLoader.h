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

        std::shared_ptr<OpenGLProgram> load(const std::string& vertexShaderName, const std::string& fragmentShaderName);

        bool tryToUpdateVertexShader(const std::shared_ptr<OpenGLProgram>& program, const std::string& shaderName);
        bool tryToUpdateFragmentShader(const std::shared_ptr<OpenGLProgram>& program, const std::string& shaderName);

    private:
        Log _logger{"ProgramLoader"};

        std::shared_ptr<AssetContentLoader> _contentLoader;

        std::string loadShaderSourceCode(const std::filesystem::path& shaderName);

        std::string compileShaderSourceCode(const std::string& code, const std::filesystem::path& includeFileDirectory);
        std::string_view parseIncludeDirectiveLine(const std::string_view& line, std::uint32_t lineNumber);
    };

}
