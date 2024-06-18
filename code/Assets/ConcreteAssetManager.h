#pragma once

#include <memory>
#include <string>

#include <Foundation/Base.h>
#include <Foundation/ObjectInMemoryCache.h>
#include "AssetManagerTypes.h"

namespace BGLRenderer
{
    class ProgramLoader;
    class TextureLoader;
    class ModelLoader;
    class MaterialLoader;

    struct ProgramShaderNames
    {
        std::string vertex;
        std::string fragment;
    };

    template<class TAssetLoader, class TAsset, class TAssetID=std::string>
    class ConcreteAssetManager
    {
    public:
        using AssetType = TAsset;
        using AssetLoaderType = TAssetLoader;

    public:
        ConcreteAssetManager(const std::shared_ptr<TAssetLoader>& assetLoader,
                             const std::shared_ptr<ObjectInMemoryCache<std::string, TAsset> >& assetCache) :
            _assetLoader(assetLoader),
            _assetCache(assetCache)
        {
        }

        inline bool exists(const TAssetID& name)
        {
            return _assetCache->exists(name);
        }

        inline const std::shared_ptr<TAssetLoader>& loader() { return _assetLoader; }

    protected:
        std::shared_ptr<TAssetLoader> _assetLoader;
        std::shared_ptr<ObjectInMemoryCache<std::string, TAsset> > _assetCache;
    };

    class TextureAssetManager : public ConcreteAssetManager<TextureLoader, OpenGLTexture2D>
    {
    public:
        TextureAssetManager(const std::shared_ptr<AssetLoaderType>& assetLoader,
                            const std::shared_ptr<ObjectInMemoryCache<std::string, AssetType> >& assetCache) :
            ConcreteAssetManager(assetLoader, assetCache)
        {
        }

        void registerAsset(const std::string& name, const std::shared_ptr<OpenGLTexture2D>& asset);

        std::shared_ptr<OpenGLTexture2D> get(const std::string& name);
        std::shared_ptr<OpenGLTexture2D> getHDR(const std::string& name);
    };

    class ProgramAssetManager : public ConcreteAssetManager<ProgramLoader, OpenGLProgram>
    {
    public:
        ProgramAssetManager(const std::shared_ptr<AssetLoaderType>& assetLoader,
                            const std::shared_ptr<ObjectInMemoryCache<std::string, AssetType> >& assetCache) :
            ConcreteAssetManager(assetLoader, assetCache)
        {
        }

        void registerAsset(const std::string& name, const std::shared_ptr<OpenGLProgram>& program);
        void registerAsset(const ProgramShaderNames& name, const std::shared_ptr<OpenGLProgram>& program);

        std::shared_ptr<OpenGLProgram> get(const ProgramShaderNames& name);

        inline bool exists(const ProgramShaderNames& name) const
        {
            const std::string programName = name.vertex + "+" + name.fragment;
            return _assetCache->exists(programName);
        }
    };

    class MaterialAssetManager : public ConcreteAssetManager<MaterialLoader, OpenGLMaterial>
    {
    public:
        MaterialAssetManager(const std::shared_ptr<AssetLoaderType>& assetLoader,
                             const std::shared_ptr<ObjectInMemoryCache<std::string, AssetType> >& assetCache) :
            ConcreteAssetManager(assetLoader, assetCache)
        {
        }

        void registerAsset(const std::string& name, const std::shared_ptr<OpenGLMaterial>& material);
        std::shared_ptr<OpenGLMaterial> get(const std::string& name);
    };

    class ModelAssetManager : public ConcreteAssetManager<ModelLoader, OpenGLRenderObject>
    {
    public:
        ModelAssetManager(const std::shared_ptr<AssetLoaderType>& assetLoader,
                          const std::shared_ptr<ObjectInMemoryCache<std::string, AssetType> >& assetCache) :
            ConcreteAssetManager(assetLoader, assetCache)
        {
        }

        void registerAsset(const std::string& name, const std::shared_ptr<OpenGLRenderObject>& model);

        std::shared_ptr<OpenGLRenderObject> get(const std::string& name);
        std::shared_ptr<OpenGLRenderObject> get(const std::string& name, const std::shared_ptr<OpenGLProgram>& program);
    };
}
