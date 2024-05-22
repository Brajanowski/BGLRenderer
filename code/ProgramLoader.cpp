﻿#include "ProgramLoader.h"

namespace BGLRenderer
{
    ProgramLoader::ProgramLoader(const std::shared_ptr<AssetContentLoader>& contentLoader) :
        _contentLoader(contentLoader)
    {
    }

    std::shared_ptr<OpenGLProgram> ProgramLoader::loadProgram(const std::string& vertexShaderName, const std::string& fragmentShaderName)
    {
        std::vector<std::uint8_t> vertexContent = _contentLoader->load(vertexShaderName);
        std::vector<std::uint8_t> fragmentContent = _contentLoader->load(fragmentShaderName);

        std::shared_ptr<OpenGLProgram> program = std::make_shared<OpenGLProgram>(std::string(vertexContent.begin(), vertexContent.end()), std::string(fragmentContent.begin(), fragmentContent.end()));
        return program;
    }
}
