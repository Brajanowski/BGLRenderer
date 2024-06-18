#include "Gizmos.h"

namespace BGLRenderer
{
    static constexpr glm::vec4 defaultGizmoColor = {1, 1, 1, 1};

    void Gizmos::wireCube(glm::vec3 center, glm::vec3 size)
    {
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), size);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), center) * scale;
        pushRender(_assets.wireCube, model);
    }

    void Gizmos::arrow(glm::vec3 origin, glm::vec3 direction, float scale)
    {
        glm::mat4 rotationMatrix(1.0f);

        if (glm::abs(glm::dot(glm::normalize(direction), {0, 1, 0})) < 0.99f)
        {
            glm::vec3 forward = glm::normalize(direction);
            glm::vec3 right = glm::normalize(glm::cross(forward, {0, 1, 0}));
            glm::vec3 up = glm::normalize(glm::cross(forward, right));

            // NOTE - forward and up axises are swapped because arrow model points towards up (+y)
            rotationMatrix = glm::mat4(glm::vec4(right, 0.0f),
                                       glm::vec4(forward, 0.0f),
                                       glm::vec4(up, 0.0f),
                                       glm::vec4(0, 0, 0, 1));
        }

        glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), origin);
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3{scale, scale, scale});

        pushRender(_assets.arrow, translateMatrix * scaleMatrix * rotationMatrix);
    }

    void Gizmos::coordinateSystem(glm::vec3 center, glm::vec3 right, glm::vec3 up, glm::vec3 forward, float scale)
    {
        pushColor({1, 0, 0, 1});
        arrow(center, right, scale);
        popColor();

        pushColor({0, 1, 0, 1});
        arrow(center, up, scale);
        popColor();

        pushColor({0, 0, 1, 1});
        arrow(center, forward, scale);
        popColor();
    }

    void Gizmos::coordinateSystem(glm::vec3 center, glm::mat4 matrix, float scale)
    {
        glm::vec3 right = glm::vec3(matrix[0]);
        glm::vec3 up = glm::vec3(matrix[1]);
        glm::vec3 forward = glm::vec3(matrix[2]);

        coordinateSystem(center, right, up, forward, scale);
    }

    void Gizmos::sphere(glm::vec3 center, float radius)
    {
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), {radius, radius, radius});
        glm::mat4 model = glm::translate(glm::mat4(1.0f), center) * scale;
        pushRender(_assets.sphere, model);
    }

    void Gizmos::render(const glm::mat4& viewProjection)
    {
        _assets.gizmosProgram->bind();
        _assets.gizmosProgram->setMatrix4x4("u_viewProjection", viewProjection);

        for (const auto& element: _renderList)
        {
            _assets.gizmosProgram->setMatrix4x4("u_model", element.model);
            _assets.gizmosProgram->setVector4("u_color", element.color);

            element.mesh->bind();
            element.mesh->draw();
        }

        _renderList.clear();
    }

    void Gizmos::pushRender(const std::shared_ptr<OpenGLMesh>& mesh, const glm::mat4& model)
    {
        GizmoRender render;
        render.mesh = mesh;
        render.color = _colorStack.empty() ? defaultGizmoColor : _colorStack.top();
        render.model = model;
        _renderList.push_back(render);
    }
}
