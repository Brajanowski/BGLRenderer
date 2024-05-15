#pragma once

#include "OpenGLBase.h"

#include <glm.hpp>

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
        void setNormals(GLfloat* normals, GLuint count);
        void setTangents(GLfloat* tangents, GLuint count);
        
        void setUVs0(GLfloat* uvs, GLuint count);
        
        void setIndices(GLuint* indices, GLuint count);

    private:
        GLuint _vertexArrayObject = 0;
        GLuint _vertexBufferObject = 0;
        GLuint _normalsBufferObject = 0;
        GLuint _tangentsBufferObject = 0;
        GLuint _uv0BufferObject = 0;
        GLuint _indicesBufferObject = 0;

        GLuint _indicesCount;
    };
}
