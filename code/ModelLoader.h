#pragma once

#include "OpenGLRenderObject.h"
#include "AssetContentLoader.h"
#include "ConcreteAssetManager.h"

#include "cgltf.h"
#include "Log.h"

namespace BGLRenderer
{
    class ModelLoader
    {
    public:
        ModelLoader(const std::shared_ptr<AssetContentLoader>& contentLoader,
                    const std::shared_ptr<TextureAssetManager>& textureAssetManager,
                    const std::shared_ptr<MaterialAssetManager>& materialAssetManager);
        ~ModelLoader() = default;

        std::shared_ptr<OpenGLRenderObject> load(const std::string& name);
        std::shared_ptr<OpenGLRenderObject> load(const std::string& name, const std::shared_ptr<OpenGLProgram>& program, const std::shared_ptr<OpenGLMaterial>& forceMaterial = nullptr);

    private:
        Log _logger{"Model Loader"};

        std::shared_ptr<AssetContentLoader> _contentLoader;
        std::shared_ptr<TextureAssetManager> _textureAssetManager;
        std::shared_ptr<MaterialAssetManager> _materialAssetManager;

        void loadMaterialFromCGLTFMaterial(const std::string& modelName, const std::shared_ptr<OpenGLMaterial>& target, const std::string& basePath, const cgltf_material* material);
        void loadCGLTFMaterialTextureToSlot(const std::string& modelName, const std::shared_ptr<OpenGLMaterial>& target, const std::string& slotName, const std::string& basePath, const cgltf_texture* texture);

        void loadAttributeDataIntoVector(std::vector<GLfloat>& data, const cgltf_attribute* attribute, int components);
        void loadTangentAttributeDataIntoVector(std::vector<GLfloat>& data, const cgltf_attribute* attribute);
    };
}
