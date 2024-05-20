#include "OpenGLRenderer.h"

namespace BGLRenderer
{
    OpenGLRenderer::OpenGLRenderer(int frameWidth, int frameHeight) :
        _frameWidth(frameWidth),
        _frameHeight(frameHeight)
    {
        
    }

    OpenGLRenderer::~OpenGLRenderer()
    {
    }

    void OpenGLRenderer::resizeFrame(int width, int height)
    {
        _frameWidth = width;
        _frameHeight = height;
    }

    void OpenGLRenderer::beginFrame()
    {
        _renderObjects.clear();
    }

    void OpenGLRenderer::endFrame()
    {
        glViewport(0, 0, _frameWidth, _frameHeight);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        GL_CALL(glEnable(GL_DEPTH_TEST));
        GL_CALL(glDepthFunc(GL_LEQUAL));

        GL_CALL(glEnable(GL_CULL_FACE));
        GL_CALL(glCullFace(GL_BACK));

        for (const auto& renderObject : _renderObjects)
        {
            glm::mat4 model = renderObject->modelMatrix();

            for (const auto& submesh : renderObject->submeshes())
            {
                submesh.material->bind();

                // TODO: cache uniform locations
                submesh.material->program()->setMatrix4x4(submesh.material->program()->getUniformLocation("u_model"), model);
                submesh.material->program()->setMatrix4x4(submesh.material->program()->getUniformLocation("u_viewProjection"), _viewProjection);
                
                submesh.mesh->bind();
                submesh.mesh->draw();
            }
        }
    }

    void OpenGLRenderer::initializeDefaultResources()
    {
        _quadMesh = std::make_shared<OpenGLMesh>();

        GLfloat vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.5f, 0.5f, 0.0f,
            -0.5f, 0.5f, 0.0f,
        };

        GLfloat normals[] = {
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f
        };

        GLfloat uvs[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };

        GLuint indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        _quadMesh->setVertices(vertices, sizeof(vertices) / sizeof(GLfloat));
        _quadMesh->setNormals(normals, sizeof(normals) / sizeof(GLfloat));
        _quadMesh->setUVs0(uvs, sizeof(uvs) / sizeof(GLfloat));
        _quadMesh->setIndices(indices, sizeof(indices) / sizeof(GLuint));
    }
}
