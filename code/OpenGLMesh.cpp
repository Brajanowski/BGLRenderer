#include "OpenGLMesh.h"

namespace BGLRenderer
{
    OpenGLMesh::OpenGLMesh()
    {
        GL_CALL(glGenVertexArrays(1, &_vertexArrayObject));
        GL_CALL(glBindVertexArray(_vertexArrayObject));

        GL_CALL(glGenBuffers(1, &_vertexBufferObject));
        GL_CALL(glGenBuffers(1, &_indicesBufferObject));
        GL_CALL(glGenBuffers(1, &_normalsBufferObject));
        GL_CALL(glGenBuffers(1, &_tangentsBufferObject));
        GL_CALL(glGenBuffers(1, &_uv0BufferObject));

        _indicesCount = 0;
    }

    OpenGLMesh::~OpenGLMesh()
    {
        GL_CALL(glDeleteVertexArrays(1, &_vertexArrayObject));
        GL_CALL(glDeleteBuffers(1, &_vertexBufferObject));
        GL_CALL(glDeleteBuffers(1, &_normalsBufferObject));
        GL_CALL(glDeleteBuffers(1, &_tangentsBufferObject));
        GL_CALL(glDeleteBuffers(1, &_uv0BufferObject));
        GL_CALL(glDeleteBuffers(1, &_indicesBufferObject));
    }

    void OpenGLMesh::bind()
    {
        GL_CALL(glBindVertexArray(_vertexArrayObject));
    }

    void OpenGLMesh::draw()
    {
        GL_CALL(glDrawElements(GL_TRIANGLES, _indicesCount, GL_UNSIGNED_INT, 0));
    }

    void OpenGLMesh::setVertices(GLfloat* vertices, GLuint count)
    {
        bind();

        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferObject));
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * count, vertices, GL_STATIC_DRAW));

        GL_CALL(glEnableVertexAttribArray(0));
        GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0));
    }

    void OpenGLMesh::setNormals(GLfloat* normals, GLuint count)
    {
        bind();

        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _normalsBufferObject));
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * count, normals, GL_STATIC_DRAW));

        GL_CALL(glEnableVertexAttribArray(1));
        GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0));
    }

    void OpenGLMesh::setTangents(GLfloat* tangents, GLuint count)
    {
        bind();

        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _tangentsBufferObject));
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * count, tangents, GL_STATIC_DRAW));

        GL_CALL(glEnableVertexAttribArray(2));
        GL_CALL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0));
    }

    void OpenGLMesh::setUVs0(GLfloat* uvs, GLuint count)
    {
        bind();

        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _uv0BufferObject));
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * count, uvs, GL_STATIC_DRAW));

        GL_CALL(glEnableVertexAttribArray(3));
        GL_CALL(glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0));
    }

    void OpenGLMesh::setIndices(GLuint* indices, GLuint count)
    {
        bind();
        GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesBufferObject));
        GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * count, indices, GL_STATIC_DRAW));

        _indicesCount = count;
    }
}
