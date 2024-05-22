#include "OpenGLMesh.h"

#include "GLMMath.h"

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

    void OpenGLMesh::calculateNormals(std::vector<GLfloat>& target, const std::vector<GLfloat>& positions, const std::vector<GLuint>& indices)
    {
        ASSERT(indices.size() % 3 == 0, "Invalid indices buffer size! Must be dividable by 3");

        int vertexCount = positions.size() / 3;
        int trianglesCount = indices.size() / 3;
        target.resize(positions.size(), 0.0f);

        for (int i = 0; i < trianglesCount; ++i)
        {
            int i0 = indices[i * 3];
            int i1 = indices[i * 3 + 1];
            int i2 = indices[i * 3 + 2];

            glm::vec3 a = {positions[i0 * 3], positions[i0 * 3 + 1], positions[i0 * 3 + 2]};
            glm::vec3 b = {positions[i1 * 3], positions[i1 * 3 + 1], positions[i1 * 3 + 2]};
            glm::vec3 c = {positions[i2 * 3], positions[i2 * 3 + 1], positions[i2 * 3 + 2]};

            glm::vec3 ab = b - a;
            glm::vec3 ac = c - a;
            glm::vec3 normal = glm::normalize(glm::cross(ab, ac));

            target[i0 * 3] += normal.x;
            target[i0 * 3 + 1] += normal.y;
            target[i0 * 3 + 2] += normal.z;

            target[i1 * 3] += normal.x;
            target[i1 * 3 + 1] += normal.y;
            target[i1 * 3 + 2] += normal.z;

            target[i2 * 3] += normal.x;
            target[i2 * 3 + 1] += normal.y;
            target[i2 * 3 + 2] += normal.z;
        }

        for (int i = 0; i < vertexCount; ++i)
        {
            glm::vec3 normal = glm::normalize(glm::vec3(target[i * 3], target[i * 3 + 1], target[i * 3 + 2]));

            target[i * 3] = normal.x;
            target[i * 3 + 1] = normal.y;
            target[i * 3 + 2] = normal.z;
        }
    }

    void OpenGLMesh::calculateTangents(std::vector<GLfloat>& target, const std::vector<GLfloat>& positions, const std::vector<GLfloat>& uvs, const std::vector<GLuint>& indices)
    {
        ASSERT(indices.size() % 3 == 0, "Invalid indices buffer size! Must be dividable by 3");

        target.resize(positions.size(), 1.0f);

        if (uvs.size() / 2 != positions.size() / 3)
        {
            openGLLogger.error("UVs is not the same size as positions! Resizing tangents vector to size of positions and returning!");
            return;
        }

        int vertexCount = positions.size() / 3;
        int trianglesCount = indices.size() / 3;
        target.resize(positions.size() / 3 * 4, 0.0f);

        std::vector<glm::vec3> tangentsSum(vertexCount, glm::vec3(0, 0, 0));
        std::vector<glm::vec3> biTangentsSum(vertexCount, glm::vec3(0, 0, 0));

        for (int i = 0; i < trianglesCount; ++i)
        {
            int i0 = indices[i * 3];
            int i1 = indices[i * 3 + 1];
            int i2 = indices[i * 3 + 2];

            glm::vec3 a = {positions[i0 * 3], positions[i0 * 3 + 1], positions[i0 * 3 + 2]};
            glm::vec3 b = {positions[i1 * 3], positions[i1 * 3 + 1], positions[i1 * 3 + 2]};
            glm::vec3 c = {positions[i2 * 3], positions[i2 * 3 + 1], positions[i2 * 3 + 2]};

            glm::vec2 uv0 = {uvs[i0 * 2], uvs[i0 * 2 + 1]};
            glm::vec2 uv1 = {uvs[i1 * 2], uvs[i1 * 2 + 1]};
            glm::vec2 uv2 = {uvs[i2 * 2], uvs[i2 * 2 + 1]};

            glm::vec3 edge1 = b - a;
            glm::vec3 edge2 = c - a;
            glm::vec2 deltaUV1 = uv1 - uv0;
            glm::vec2 deltaUV2 = uv2 - uv0;

            float determinant = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

            glm::vec3 tangent;
            tangent.x = determinant * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = determinant * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = determinant * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
            tangent = glm::normalize(tangent);

            tangentsSum[i0] += tangent;
            tangentsSum[i1] += tangent;
            tangentsSum[i2] += tangent;
        }

        for (int i = 0; i < vertexCount; ++i)
        {
            glm::vec3 tangent = glm::normalize(tangentsSum[i]);

            float handedness = 1.0f;

            target[i * 4] = tangent.x;
            target[i * 4 + 1] = tangent.y;
            target[i * 4 + 2] = tangent.z;
            target[i * 4 + 3] = handedness;
        }
    }
}
