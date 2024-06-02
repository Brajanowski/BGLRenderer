#include "AssetManager.h"

namespace BGLRenderer
{
    namespace Private
    {
        static Log logger{"AssetsManager"};
    }

    AssetManager::AssetManager(const std::shared_ptr<AssetContentLoader>& contentLoader) :
        _contentLoader(contentLoader),
        _assetFileChangesObserver(contentLoader),
        _programAssetManager(std::make_shared<ProgramAssetManager>(std::make_shared<ProgramLoader>(_contentLoader), std::make_shared<ObjectInMemoryCache<std::string, OpenGLProgram>>())),
        _textureAssetManager(std::make_shared<TextureAssetManager>(std::make_shared<TextureLoader>(_contentLoader), std::make_shared<ObjectInMemoryCache<std::string, OpenGLTexture2D>>())),
        _materialAssetManager(std::make_shared<MaterialAssetManager>(std::make_shared<MaterialLoader>(_contentLoader, _textureAssetManager, _programAssetManager), std::make_shared<ObjectInMemoryCache<std::string, OpenGLMaterial>>())),
        _modelAssetManager(std::make_shared<ModelAssetManager>(std::make_shared<ModelLoader>(_contentLoader, _textureAssetManager, _materialAssetManager), std::make_shared<ObjectInMemoryCache<std::string, OpenGLRenderObject>>())),
        _sceneLoader(_contentLoader, _modelAssetManager, _materialAssetManager)
    {
    }

    void AssetManager::tick()
    {
        _assetFileChangesObserver.tick();
    }

    void AssetManager::registerAsset(const std::string& name, const std::shared_ptr<OpenGLTexture2D>& texture)
    {
        _textureAssetManager->registerAsset(name, texture);
    }

    void AssetManager::registerAsset(const std::string& name, const std::shared_ptr<OpenGLProgram>& program)
    {
        _programAssetManager->registerAsset(name, program);
    }

    void AssetManager::registerAsset(const std::string& name, const std::shared_ptr<OpenGLRenderObject>& renderObject)
    {
        _modelAssetManager->registerAsset(name, renderObject);
    }

    void AssetManager::registerAsset(const std::string& name, const std::shared_ptr<OpenGLMaterial>& material)
    {
        _materialAssetManager->registerAsset(name, material);
    }

    std::shared_ptr<OpenGLProgram> AssetManager::getProgram(const std::string& name)
    {
        return getProgram(name + ".vert", name + ".frag");
    }

    std::shared_ptr<OpenGLProgram> AssetManager::getProgram(const std::string& vertexShaderName, const std::string& fragmentShaderName)
    {
        ProgramShaderNames names{vertexShaderName, fragmentShaderName};

        if (_programAssetManager->exists(names))
        {
            return _programAssetManager->get(names);
        }

        std::shared_ptr<OpenGLProgram> program = _programAssetManager->get(names);
        addProgramShadersListeners(program, names.vertex, names.fragment);
        return program;
    }

    std::shared_ptr<OpenGLTexture2D> AssetManager::getTexture2D(const std::string& name)
    {
        return _textureAssetManager->get(name);
    }

    std::shared_ptr<OpenGLRenderObject> AssetManager::getModel(const std::string& name, const std::shared_ptr<OpenGLProgram>& program)
    {
        return _modelAssetManager->get(name, program);
    }

    std::shared_ptr<OpenGLMaterial> AssetManager::getMaterial(const std::string& name)
    {
        if (_materialAssetManager->exists(name))
        {
            return _materialAssetManager->get(name);
        }

        std::shared_ptr<OpenGLMaterial> material = _materialAssetManager->get(name);
        addMaterialListener(material, name);
        return material;
    }

    std::shared_ptr<Scene> AssetManager::getScene(const std::string& name)
    {
        std::shared_ptr<Scene> scene = _sceneLoader.load(name);
        addSceneListener(scene, name);
        return scene;
    }

    Log& AssetManager::logger()
    {
        return Private::logger;
    }

    void AssetManager::addProgramShadersListeners(const std::shared_ptr<OpenGLProgram>& program, const std::string& vertexShaderName, const std::string& fragmentShaderName)
    {
        _assetFileChangesObserver.listenFileChanged(vertexShaderName, [program=program, &logger=logger(), &programAssetManager=_programAssetManager](const AssetFileChangedEvent& ev)
        {
            std::string shaderName = ev.path.string();
            logger.debug("Vertex shader file changed, trying to update: {}", shaderName);
            programAssetManager->loader()->tryToUpdateVertexShader(program, shaderName);
        });

        _assetFileChangesObserver.listenFileChanged(fragmentShaderName, [program=program, &logger=logger(), &programAssetManager=_programAssetManager](const AssetFileChangedEvent& ev)
        {
            std::string shaderName = ev.path.string();
            logger.debug("Fragment shader file changed, trying to update: {}", shaderName);
            programAssetManager->loader()->tryToUpdateFragmentShader(program, shaderName);
        });
    }

    void AssetManager::addMaterialListener(const std::shared_ptr<OpenGLMaterial>& material, const std::string& name)
    {
        ASSERT(material != nullptr, "Cannot listen for null material changes");

        _assetFileChangesObserver.listenFileChanged(name, [material=material, &logger=logger(), &materialAssetManager=_materialAssetManager](const AssetFileChangedEvent& ev)
        {
            std::string materialName = ev.path.string();
            logger.debug("Material file changed, trying to update: {}", materialName);

            materialAssetManager->loader()->tryToUpdateMaterial(material, materialName);
        });
    }

    void AssetManager::addSceneListener(const std::shared_ptr<Scene>& scene, const std::string& name)
    {
        ASSERT(scene != nullptr, "Cannot listen for null scene changes");

        _assetFileChangesObserver.listenFileChanged(name, [scene=scene, &sceneLoader=_sceneLoader](const AssetFileChangedEvent& ev)
        {
            sceneLoader.loadInto(scene, ev.path.string());
        });
    }
}
