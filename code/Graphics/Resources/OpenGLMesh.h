#pragma once

#include "../OpenGLBase.h"

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

        [[nodiscard]] const std::vector<GLfloat>& positions() const { return _positions; }
        [[nodiscard]] const std::vector<GLfloat>& normals() const { return _normals; }
        [[nodiscard]] const std::vector<GLfloat>& tangents() const { return _tangents; }
        [[nodiscard]] const std::vector<GLfloat>& uvs() const { return _uvs; }
        [[nodiscard]] const std::vector<GLuint>& indices() const { return _indices; }

    private:
        GLuint _vertexArrayObject = 0;
        GLuint _vertexBufferObject = 0;
        GLuint _normalsBufferObject = 0;
        GLuint _tangentsBufferObject = 0;
        GLuint _uv0BufferObject = 0;
        GLuint _indicesBufferObject = 0;

        GLuint _indicesCount;

        // TODO: make these values optional
        std::vector<GLfloat> _positions;
        std::vector<GLfloat> _normals;
        std::vector<GLfloat> _tangents;
        std::vector<GLfloat> _uvs;
        std::vector<GLuint> _indices;

    public:
        static void calculateNormals(std::vector<GLfloat>& target, const std::vector<GLfloat>& positions, const std::vector<GLuint>& indices);
        static void calculateTangents(std::vector<GLfloat>& target,
                                      const std::vector<GLfloat>& positions,
                                      const std::vector<GLfloat>& normals,
                                      const std::vector<GLfloat>& uvs,
                                      const std::vector<GLuint>& indices);
    };
}
