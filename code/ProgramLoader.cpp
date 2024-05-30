#include "ProgramLoader.h"

namespace BGLRenderer
{
    ProgramLoader::ProgramLoader(const std::shared_ptr<AssetContentLoader>& contentLoader) :
        _contentLoader(contentLoader)
    {
    }

    std::shared_ptr<OpenGLProgram> ProgramLoader::load(const std::string& vertexShaderName, const std::string& fragmentShaderName)
    {
        std::string vertexShaderCode = loadShaderSourceCode(vertexShaderName);
        std::string fragmentShaderCode = loadShaderSourceCode(fragmentShaderName);

        std::shared_ptr<OpenGLProgram> program = std::make_shared<OpenGLProgram>(std::format("{}+{}", vertexShaderName, fragmentShaderName), vertexShaderCode, fragmentShaderCode);
        return program;
    }

    bool ProgramLoader::tryToUpdateVertexShader(const std::shared_ptr<OpenGLProgram>& program, const std::string& shaderName)
    {
        std::vector<std::uint8_t> shaderContent = _contentLoader->load(shaderName);

        std::string compiledShaderSource = loadShaderSourceCode(std::string(shaderContent.begin(), shaderContent.end()));

        if (compiledShaderSource.empty())
        {
            return false;
        }

        return program->tryToUpdateVertexShader(compiledShaderSource);
    }

    bool ProgramLoader::tryToUpdateFragmentShader(const std::shared_ptr<OpenGLProgram>& program, const std::string& shaderName)
    {
        std::vector<std::uint8_t> shaderContent = _contentLoader->load(shaderName);

        std::string compiledShaderSource = loadShaderSourceCode(std::string(shaderContent.begin(), shaderContent.end()));

        if (compiledShaderSource.empty())
        {
            return false;
        }

        return program->tryToUpdateFragmentShader(compiledShaderSource);
    }

    std::string ProgramLoader::loadShaderSourceCode(const std::filesystem::path& shaderName)
    {
        std::vector<std::uint8_t> shaderContent = _contentLoader->load(shaderName);
        std::string shaderContentString = std::string(shaderContent.begin(), shaderContent.end());

        return compileShaderSourceCode(shaderContentString, shaderName.parent_path());
    }

    std::string ProgramLoader::compileShaderSourceCode(const std::string& inputCode, const std::filesystem::path& includeFileDirectory)
    {
        std::size_t lineStart = 0;
        std::uint32_t lineNumber = 0;

        std::string code;
        std::ranges::remove_copy(inputCode, std::back_inserter(code), '\r');

        std::string result{};
        result.reserve(code.length());

        for (std::size_t i = 0; i <= code.length(); ++i)
        {
            if (code[i] == '\n' || i == code.length())
            {
                std::string_view line{code.begin() + lineStart, code.begin() + i};

                if (i == lineStart)
                {
                    result += '\n';
                }
                else if (line.starts_with("#include"))
                {
                    std::filesystem::path includePath = includeFileDirectory / parseIncludeDirectiveLine(line, lineNumber);

                    if (includePath.empty())
                    {
                        _logger.error("Failed to parse include directive at line {}", lineNumber);
                    }
                    else
                    {
                        std::string includeFileCompiledSource = loadShaderSourceCode(includePath);

                        result += std::format("/// MARK - begin of \"{}\" include file\n", includePath.string());
                        result += includeFileCompiledSource;
                        result += std::format("/// MARK - end of \"{}\" include file\n", includePath.string());
                    }
                }
                else
                {
                    result += line;
                    result += '\n';
                }

                lineStart = i + 1;
                lineNumber++;
            }
        }

        return result;
    }

    std::string_view ProgramLoader::parseIncludeDirectiveLine(const std::string_view& line, std::uint32_t lineNumber)
    {
        std::size_t quotationMarkCount = std::ranges::count_if(line, [](char c)
        {
            return c == '"';
        });

        if (quotationMarkCount > 2)
        {
            _logger.error("Nested quotation marks are not allowed, error at line: {}", lineNumber);
            return {};
        }

        if (quotationMarkCount < 2)
        {
            _logger.error("Missing open or close quotation mark, error at line: {}", lineNumber);
            return {};
        }

        std::string_view includePath(line.begin() + line.find_first_of('"') + 1, line.begin() + line.find_last_of('"'));

        if (includePath.empty())
        {
            _logger.error("Empty include path, error at line: " + lineNumber);
            return {};
        }

        return includePath;
    }
}
