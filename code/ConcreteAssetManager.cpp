#include "ConcreteAssetManager.h"

#include "AssetManager.h"

#include "MaterialLoader.h"
#include "ProgramLoader.h"
#include "ModelLoader.h"
#include "TextureLoader.h"

namespace BGLRenderer
{
    void TextureAssetManager::registerAsset(const std::string& name, const std::shared_ptr<OpenGLTexture2D>& asset)
    {
        if (_assetCache->exists(name))
        {
            AssetManager::logger().error("Texture with given name \"{}\" is already registered!", name);
            return;
        }

        _assetCache->set(name, asset);
    }

    std::shared_ptr<OpenGLTexture2D> TextureAssetManager::get(const std::string& name)
    {
        if (_assetCache->exists(name))
        {
            return _assetCache->get(name);
        }

        AssetManager::logger().debug("Loading texture from: {}", name);

        std::shared_ptr<OpenGLTexture2D> texture = _assetLoader->loadTexture2D(name);
        if (texture == nullptr)
        {
            AssetManager::logger().error("Couldn't find texture2d: \"{}\"", name);
            return nullptr;
        }

        registerAsset(name, texture);

        return texture;
    }

    void ProgramAssetManager::registerAsset(const std::string& name, const std::shared_ptr<OpenGLProgram>& program)
    {
        if (_assetCache->exists(name))
        {
            AssetManager::logger().error("Program with given name \"{}\" is already registered!", name);
            return;
        }

        _assetCache->set(name, program);
    }

    void ProgramAssetManager::registerAsset(const ProgramShaderNames& name, const std::shared_ptr<OpenGLProgram>& program)
    {
        std::string assetName = name.vertex + "+" + name.fragment;
        registerAsset(assetName, program);
    }

    std::shared_ptr<OpenGLProgram> ProgramAssetManager::get(const ProgramShaderNames& name)
    {
        std::string programName = name.vertex + "+" + name.fragment;

        if (_assetCache->exists(programName))
        {
            return _assetCache->get(programName);
        }

        std::shared_ptr<OpenGLProgram> program = _assetLoader->load(name.vertex, name.fragment);
        registerAsset(programName, program);
        return program;
    }

    void MaterialAssetManager::registerAsset(const std::string& name, const std::shared_ptr<OpenGLMaterial>& material)
    {
        if (_assetCache->exists(name))
        {
            AssetManager::logger().error("Material with given name \"{}\" is already registered!", name);
            return;
        }

        _assetCache->set(name, material);
    }

    std::shared_ptr<OpenGLMaterial> MaterialAssetManager::get(const std::string& name)
    {
        if (_assetCache->exists(name))
        {
            return _assetCache->get(name);
        }

        std::shared_ptr<OpenGLMaterial> material = _assetLoader->load(name);

        if (material == nullptr)
        {
            AssetManager::logger().error("Couldn't find material: \"{}\"", name);
            return nullptr;
        }

        registerAsset(name, material);
        return material;
    }

    void ModelAssetManager::registerAsset(const std::string& name, const std::shared_ptr<OpenGLRenderObject>& model)
    {
        if (_assetCache->exists(name))
        {
            AssetManager::logger().error("Model with given name \"{}\" is already registered!", name);
            return;
        }

        _assetCache->set(name, model);
    }

    std::shared_ptr<OpenGLRenderObject> ModelAssetManager::get(const std::string& name)
    {
        if (_assetCache->exists(name))
        {
            return _assetCache->get(name);
        }
        
        std::shared_ptr<OpenGLRenderObject> renderObject = _assetLoader->load(name);
        if (renderObject == nullptr)
        {
            AssetManager::logger().error("Couldn't load model: \"{}\"", name);
            return nullptr;
        }

        registerAsset(name, renderObject);
        return renderObject;
    }

    std::shared_ptr<OpenGLRenderObject> ModelAssetManager::get(const std::string& name, const std::shared_ptr<OpenGLProgram>& program)
    {
        if (_assetCache->exists(name))
        {
            return _assetCache->get(name);
        }

        std::shared_ptr<OpenGLRenderObject> renderObject = _assetLoader->load(name, program);
        if (renderObject == nullptr)
        {
            AssetManager::logger().error("Couldn't find model: \"{}\"", name);
            return nullptr;
        }

        registerAsset(name, renderObject);
        return renderObject;
    }
}
