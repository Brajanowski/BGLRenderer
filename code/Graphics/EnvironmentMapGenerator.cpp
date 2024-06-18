#include "EnvironmentMapGenerator.h"

#include "OpenGLRenderer.h"

namespace BGLRenderer
{
    EnvironmentMapGenerator::EnvironmentMapGenerator(OpenGLRenderer* renderer,
                                                     const std::shared_ptr<AssetManager>& assetManager) :
        _renderer(renderer),
        _assetManager(assetManager),
        _resourcesLoaded(false)
    {
    }

    void EnvironmentMapGenerator::loadDefaultResources()
    {
        _logger.debug("Loading default resources");

        constexpr const char* processingVert = "shaders/cubemap_processing.vert";

        _quad = _renderer->quadMesh();
        _equirectangularToCubemap = _assetManager->getProgram(processingVert,
                                                              "shaders/equirectangular_to_cubemap.frag");
        _irradianceCubemapGenerator = _assetManager->getProgram(processingVert,
                                                                "shaders/irradiance_cubemap_generator.frag");

        _framebuffer = std::make_shared<OpenGLFramebuffer>("Environment Map Generator", 2048, 2048);
        //_framebuffer->createDepthAttachment(GL_DEPTH_COMPONENT);

        _resourcesLoaded = true;
    }

    void EnvironmentMapGenerator::generate(const std::shared_ptr<OpenGLEnvironmentMap>& environmentMap,
                                           const std::shared_ptr<OpenGLTexture2D>& equirectangularMap)
    {
        if (!_resourcesLoaded)
        {
            loadDefaultResources();
        }

        _logger.debug("Generating environment map \"{}\" from equirectangular map \"{}\"",
                      environmentMap->name().c_str(),
                      equirectangularMap->name().c_str());

        auto bindEquirectangularMap = [&](const std::shared_ptr<OpenGLProgram>& program)
        {
            equirectangularMap->bind(1);
            program->setInt("u_texture", 1);
        };
        process(environmentMap->cubemap(), _equirectangularToCubemap, bindEquirectangularMap);

        auto bindCubemap = [&](const std::shared_ptr<OpenGLProgram>& program)
        {
            environmentMap->cubemap()->bind(1);
            program->setInt("u_texture", 1);
        };

        process(environmentMap->irradianceMap(), _irradianceCubemapGenerator, bindCubemap);
    }

    void EnvironmentMapGenerator::process(const std::shared_ptr<OpenGLCubemap>& target,
                                          const std::shared_ptr<OpenGLProgram>& program,
                                          const BindUniformsFn& bindUniformsFn)
    {
        static const glm::mat4 captureViews[] =
        {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
        };

        GLuint resolution = target->faceSize();
        _framebuffer->resize(resolution, resolution);

        GL_CALL(glDisable(GL_DEPTH_TEST));
        GL_CALL(glDisable(GL_BLEND));

        glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

        program->bind();
        program->setMatrix4x4("u_projection", projection);
        program->setMatrix4x4("u_view", glm::mat4(1.0f));

        bindUniformsFn(program);

        _framebuffer->bind();
        _framebuffer->clearColorAttachments();

        GL_CALL(glViewport(0, 0, resolution, resolution));

        constexpr GLenum drawTargets[] = {
            GL_COLOR_ATTACHMENT0
        };
        GL_CALL(glDrawBuffers(1, drawTargets));

        for (int i = 0; i < 6; ++i)
        {
            _framebuffer->addColorCubeFace(target, static_cast<OpenGLCubeFace>(i));

            program->setMatrix4x4("u_view", captureViews[i]);

            _quad->bind();
            _quad->draw();

            _framebuffer->clearColorAttachments();
        }

        _framebuffer->unbind();
    }
}
