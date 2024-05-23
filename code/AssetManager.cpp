#include "AssetManager.h"

namespace BGLRenderer
{
    AssetManager::AssetManager(const std::shared_ptr<AssetContentLoader>& contentLoader) :
        _contentLoader(contentLoader),
        _assetFileChangesObserver(contentLoader),
        _programCache(std::make_shared<ObjectInMemoryCache<std::string, OpenGLProgram>>()),
        _texture2DCache(std::make_shared<ObjectInMemoryCache<std::string, OpenGLTexture2D>>()),
        _modelCache(std::make_shared<ObjectInMemoryCache<std::string, OpenGLRenderObject>>()),
        _programLoader(std::make_shared<ProgramLoader>(contentLoader)),
        _textureLoader(std::make_shared<TextureLoader>(contentLoader)),
        _modelLoader(std::make_shared<ModelLoader>(contentLoader, _textureLoader, _texture2DCache))
    {
    }

    void AssetManager::tick()
    {
        _assetFileChangesObserver.tick();
    }

    void AssetManager::registerTexture(const std::string& name, const std::shared_ptr<OpenGLTexture2D>& texture)
    {
        if (_texture2DCache->exists(name))
        {
            _logger.error("Texture with given name \"{}\" is already registered!", name);
            return;
        }

        _texture2DCache->set(name, texture);
    }

    void AssetManager::registerProgram(const std::string& name, const std::shared_ptr<OpenGLProgram>& program)
    {
        if (_programCache->exists(name))
        {
            _logger.error("Program with given name \"{}\" is already registered!", name);
            return;
        }

        _programCache->set(name, program);
    }

    void AssetManager::registerModel(const std::string& name, const std::shared_ptr<OpenGLRenderObject>& renderObject)
    {
        if (_modelCache->exists(name))
        {
            _logger.error("Model with given name \"{}\" is already registered!", name);
            return;
        }

        _modelCache->set(name, renderObject);
    }

    std::shared_ptr<OpenGLProgram> AssetManager::getProgram(const std::string& name)
    {
        if (_programCache->exists(name))
        {
            return _programCache->get(name);
        }

        std::string vertexShaderName = name + ".vert";
        std::string fragmentShaderName = name + ".frag";

        std::shared_ptr<OpenGLProgram> program = _programLoader->loadProgram(vertexShaderName, fragmentShaderName);
        registerProgram(name, program);
        addProgramShadersListeners(program, vertexShaderName, fragmentShaderName);
        return program;
    }

    std::shared_ptr<OpenGLProgram> AssetManager::getProgram(const std::string& vertexShaderName, const std::string& fragmentShaderName)
    {
        std::string programName = vertexShaderName + "+" + fragmentShaderName;

        if (_programCache->exists(programName))
        {
            return _programCache->get(programName);
        }

        std::shared_ptr<OpenGLProgram> program = _programLoader->loadProgram(vertexShaderName, fragmentShaderName);
        registerProgram(programName, program);
        addProgramShadersListeners(program, vertexShaderName, fragmentShaderName);
        return program;
    }

    std::shared_ptr<OpenGLTexture2D> AssetManager::getTexture(const std::string& name)
    {
        if (_texture2DCache->exists(name))
        {
            return _texture2DCache->get(name);
        }

        _logger.debug("Loading texture from: {}", name);

        std::shared_ptr<OpenGLTexture2D> texture = _textureLoader->loadTexture2D(name);
        registerTexture(name, texture);
        return texture;
    }

    std::shared_ptr<OpenGLRenderObject> AssetManager::getModel(const std::string& name, const std::shared_ptr<OpenGLProgram>& program)
    {
        if (_modelCache->exists(name))
        {
            return _modelCache->get(name);
        }

        std::shared_ptr<OpenGLRenderObject> renderObject = _modelLoader->loadModel(name, program);
        registerModel(name, renderObject);
        return renderObject;
    }

    void AssetManager::addProgramShadersListeners(const std::shared_ptr<OpenGLProgram>& program, const std::string& vertexShaderName, const std::string& fragmentShaderName)
    {
        _assetFileChangesObserver.listenFileChanged(vertexShaderName, [program=program, &logger=_logger, &programLoader=_programLoader](const std::filesystem::path& path, const std::filesystem::file_time_type& time)
        {
            std::string shaderName = path.string();
            logger.debug("Vertex shader file changed, trying to update: {}", shaderName);
            programLoader->tryToUpdateVertexShader(program, shaderName);
        });

        _assetFileChangesObserver.listenFileChanged(fragmentShaderName, [program=program, &logger=_logger, &programLoader=_programLoader](const std::filesystem::path& path, const std::filesystem::file_time_type& time)
        {
            std::string shaderName = path.string();
            logger.debug("Fragment shader file changed, trying to update: {}", shaderName);
            programLoader->tryToUpdateFragmentShader(program, shaderName);
        });
    }
}
