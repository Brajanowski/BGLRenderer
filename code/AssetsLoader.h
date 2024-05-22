#pragma once

#include "OpenGLBase.h"
#include "AssetContentLoader.h"
#include "OpenGLProgram.h"
#include "OpenGLRenderObject.h"
#include "OpenGLTexture2D.h"

#include "cgltf.h"

namespace BGLRenderer
{
    class AssetsLoader
    {
    public:
        AssetsLoader(const std::shared_ptr<AssetContentLoader>& contentLoader);

        void registerTexture(const std::string& name, const std::shared_ptr<OpenGLTexture2D>& texture);
        
        std::shared_ptr<OpenGLProgram> loadProgram(const std::string& name);
        std::shared_ptr<OpenGLProgram> loadProgram(const std::string& vertexShaderName, const std::string& fragmentShaderName);

        std::shared_ptr<OpenGLTexture2D> loadTexture(const std::string& name);

        std::shared_ptr<OpenGLRenderObject> loadModel(const std::string& name, const std::shared_ptr<OpenGLProgram>& program);

    private:
        Log _logger{"AssetsLoader"};
        std::shared_ptr<AssetContentLoader> _contentLoader;

        std::map<std::string, std::shared_ptr<OpenGLProgram>> _loadedPrograms;
        std::map<std::string, std::shared_ptr<OpenGLTexture2D>> _loadedTextures;

        inline bool isTextureLoaded(const std::string& name) { return _loadedTextures.contains(name); }

        std::shared_ptr<OpenGLTexture2D> loadTextureFromImageData(const std::vector<std::uint8_t>& imageFileContent);
        std::shared_ptr<OpenGLTexture2D> loadTextureFromImageData(const std::uint8_t* bytes, size_t size);
        void loadMaterialFromCGLTFMaterial(const std::string& modelName, const std::shared_ptr<OpenGLMaterial>& target, const std::string& basePath, const cgltf_material* material);
        void loadCGLTFMaterialTextureToSlot(const std::string& modelName, const std::shared_ptr<OpenGLMaterial>& target, const std::string& slotName, const std::string& basePath, const cgltf_texture* texture);

        void loadAttributeDataIntoVector(std::vector<GLfloat>& data, const cgltf_attribute* attribute, int components);
        void loadTangentAttributeDataIntoVector(std::vector<GLfloat>& data, const cgltf_attribute* attribute);
    };
}
