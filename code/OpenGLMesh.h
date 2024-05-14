#pragma once

#include "OpenGLBase.h"

namespace BGLRenderer
{
    class OpenGLMesh
    {
    public:
        OpenGLMesh();
        ~OpenGLMesh();

        void bind();

        void draw();

        void setVertices(GLfloat* vertices, GLuint count);
        void setIndices(GLuint* indices, GLuint count);

    private:
        GLuint _vertexArrayObject = 0;
        GLuint _vertexBufferObject = 0;
        GLuint _indicesBufferObject = 0;

        GLuint _indicesCount;
    };
}
