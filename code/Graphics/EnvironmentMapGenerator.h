#pragma once

#include <memory>

#include "Resources/OpenGLEnvironmentMap.h"

namespace BGLRenderer
{
    class OpenGLProgram;
    class OpenGLRenderer;
    class AssetManager;
    class OpenGLMesh;
    class OpenGLFramebuffer;

    class EnvironmentMapGenerator
    {
    public:
        EnvironmentMapGenerator(OpenGLRenderer* renderer, const std::shared_ptr<AssetManager>& assetManager);

        void loadDefaultResources();

        void generate(const std::shared_ptr<OpenGLEnvironmentMap>& environmentMap,
                      const std::shared_ptr<OpenGLTexture2D>& equirectangularMap);

    private:
        using BindUniformsFn = std::function<void(const std::shared_ptr<OpenGLProgram>& program)>;

        Log _logger{"Environment Map Generator"};

        OpenGLRenderer* _renderer;
        std::shared_ptr<AssetManager> _assetManager;

        bool _resourcesLoaded;

        std::shared_ptr<OpenGLMesh> _quad;
        std::shared_ptr<OpenGLProgram> _equirectangularToCubemap;
        std::shared_ptr<OpenGLProgram> _irradianceCubemapGenerator;
        std::shared_ptr<OpenGLFramebuffer> _framebuffer;

        void process(const std::shared_ptr<OpenGLCubemap>& target,
                     const std::shared_ptr<OpenGLProgram>& program,
                     const BindUniformsFn& bindUniformsFn);
    };
}
