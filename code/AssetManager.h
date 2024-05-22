#pragma once

#include "AssetContentLoader.h"
#include "TextureLoader.h"
#include "ProgramLoader.h"
#include "ModelLoader.h"

#include "ObjectInMemoryCache.h"

#include "OpenGLBase.h"
#include "OpenGLProgram.h"
#include "OpenGLRenderObject.h"
#include "OpenGLTexture2D.h"

#include <string>

namespace BGLRenderer
{
    class AssetManager
    {
    public:
        AssetManager(const std::shared_ptr<AssetContentLoader>& contentLoader);

        void registerTexture(const std::string& name, const std::shared_ptr<OpenGLTexture2D>& texture);
        void registerProgram(const std::string& name, const std::shared_ptr<OpenGLProgram>& program);
        void registerModel(const std::string& name, const std::shared_ptr<OpenGLRenderObject>& renderObject);

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
        std::shared_ptr<OpenGLTexture2D> getTexture(const std::string& name);

        /// @brief Loads or return existing model with given name, uses given program to create materials when loading model
        std::shared_ptr<OpenGLRenderObject> getModel(const std::string& name, const std::shared_ptr<OpenGLProgram>& program);

    private:
        Log _logger{"AssetsLoader"};
        std::shared_ptr<AssetContentLoader> _contentLoader;

        std::shared_ptr<ObjectInMemoryCache<std::string, OpenGLProgram>> _programCache;
        std::shared_ptr<ObjectInMemoryCache<std::string, OpenGLTexture2D>> _texture2DCache;
        std::shared_ptr<ObjectInMemoryCache<std::string, OpenGLRenderObject>> _modelCache;
        
        std::shared_ptr<ProgramLoader> _programLoader;
        std::shared_ptr<TextureLoader> _textureLoader;
        std::shared_ptr<ModelLoader> _modelLoader;
    };
}
