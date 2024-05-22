#pragma once

#include "OpenGLRenderObject.h"
#include "AssetContentLoader.h"

#include "cgltf.h"
#include "TextureLoader.h"
#include "ObjectInMemoryCache.h"

#include "Log.h"

namespace BGLRenderer
{
    class ModelLoader
    {
    public:
        ModelLoader(const std::shared_ptr<AssetContentLoader>& contentLoader,
                    const std::shared_ptr<TextureLoader>& textureLoader,
                    const std::shared_ptr<ObjectInMemoryCache<std::string, OpenGLTexture2D>>& texturesCache);
        ~ModelLoader() = default;

        std::shared_ptr<OpenGLRenderObject> loadModel(const std::string& name, const std::shared_ptr<OpenGLProgram>& program);

    private:
        Log _logger{"Model Loader"};

        std::shared_ptr<AssetContentLoader> _contentLoader;
        std::shared_ptr<TextureLoader> _textureLoader;
        std::shared_ptr<ObjectInMemoryCache<std::string, OpenGLTexture2D>> _texturesCache;

        void loadMaterialFromCGLTFMaterial(const std::string& modelName, const std::shared_ptr<OpenGLMaterial>& target, const std::string& basePath, const cgltf_material* material);
        void loadCGLTFMaterialTextureToSlot(const std::string& modelName, const std::shared_ptr<OpenGLMaterial>& target, const std::string& slotName, const std::string& basePath, const cgltf_texture* texture);

        void loadAttributeDataIntoVector(std::vector<GLfloat>& data, const cgltf_attribute* attribute, int components);
        void loadTangentAttributeDataIntoVector(std::vector<GLfloat>& data, const cgltf_attribute* attribute);
    };
}
