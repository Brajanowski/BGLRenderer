#pragma once

#include "AssetContentLoader.h"
#include "TextureLoader.h"
#include "ProgramLoader.h"
#include "ModelLoader.h"
#include "AssetFileChangesObserver.h"
#include "ConcreteAssetManager.h"
#include "MaterialLoader.h"

#include "AssetManagerTypes.h"

#include "OpenGLProgram.h"
#include "OpenGLRenderObject.h"
#include "OpenGLTexture2D.h"

#include <string>

#include "Scene.h"
#include "SceneLoader.h"

namespace BGLRenderer
{
    class AssetManager
    {
    public:
        AssetManager(const std::shared_ptr<AssetContentLoader>& contentLoader);

        void tick();

        void registerAsset(const std::string& name, const std::shared_ptr<OpenGLTexture2D>& texture);
        void registerAsset(const std::string& name, const std::shared_ptr<OpenGLProgram>& program);
        void registerAsset(const std::string& name, const std::shared_ptr<OpenGLRenderObject>& renderObject);
        void registerAsset(const std::string& name, const std::shared_ptr<OpenGLMaterial>& material);

        /// @brief Loads or returns existing program with given name.
        /// Names of vertex and fragment shaders are constructed from the name parameter by adding appropriate suffix.
        /// If the program is successfully loaded, it's registered using the registerProgram function with the specified name.
        std::shared_ptr<OpenGLProgram> getProgram(const std::string& name);

        /// @brief Loads or returns existing program.
        /// The loaded program is registered using the registerProgram function
        /// Name is generated using provided vertex and fragment shaders names.
        /// E.g. Vertex shader name is "shaders/basic.vert" and fragment shader name is "shaders/basic.frag",
        /// the generated name will be: "shaders/basic.vert+shaders/basic.frag"
        std::shared_ptr<OpenGLProgram> getProgram(const std::string& vertexShaderName, const std::string& fragmentShaderName);

        /// @brief Loads or return existing model with given name
        std::shared_ptr<OpenGLTexture2D> getTexture2D(const std::string& name);

        /// @brief Loads or return existing model with given name, uses given program to create materials when loading model
        std::shared_ptr<OpenGLRenderObject> getModel(const std::string& name, const std::shared_ptr<OpenGLProgram>& program);

        std::shared_ptr<OpenGLMaterial> getMaterial(const std::string& name);

        std::shared_ptr<Scene> getScene(const std::string& name);

        static Log& logger();

    private:
        std::shared_ptr<AssetContentLoader> _contentLoader;
        AssetFileChangesObserver _assetFileChangesObserver;

        std::shared_ptr<ProgramAssetManager> _programAssetManager;
        std::shared_ptr<TextureAssetManager> _textureAssetManager;
        std::shared_ptr<MaterialAssetManager> _materialAssetManager;
        std::shared_ptr<ModelAssetManager> _modelAssetManager;
        SceneLoader _sceneLoader;

        void addProgramShadersListeners(const std::shared_ptr<OpenGLProgram>& program, const std::string& vertexShaderName, const std::string& fragmentShaderName);
        void addMaterialListener(const std::shared_ptr<OpenGLMaterial>& material, const std::string& name);
        void addSceneListener(const std::shared_ptr<Scene>& scene, const std::string& name);
    };
}
