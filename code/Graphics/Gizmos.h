#pragma once

#include <memory>
#include <stack>

#include <Foundation/Base.h>
#include <Foundation/GLMMath.h>
#include "Resources/OpenGLProgram.h"
#include "Resources/OpenGLMesh.h"

namespace BGLRenderer
{
    struct GizmosAssets
    {
        std::shared_ptr<OpenGLProgram> gizmosProgram;
        std::shared_ptr<OpenGLMesh> wireCube;
        std::shared_ptr<OpenGLMesh> arrow;
        std::shared_ptr<OpenGLMesh> sphere;
    };

    struct GizmoRender
    {
        std::shared_ptr<OpenGLMesh> mesh;
        glm::mat4 model;
        glm::vec4 color;
    };

    class Gizmos
    {
    public:
        void wireCube(glm::vec3 center, glm::vec3 size);
        void arrow(glm::vec3 origin, glm::vec3 direction, float scale = 1.0f);
        void coordinateSystem(glm::vec3 center, glm::vec3 right, glm::vec3 up, glm::vec3 forward, float scale = 1.0f);
        void coordinateSystem(glm::vec3 center, glm::mat4 matrix, float scale = 1.0f);

        void sphere(glm::vec3 center, float radius = 1.0f);

        inline void pushColor(glm::vec4 color) { _colorStack.push(color); }
        inline void popColor() { _colorStack.pop(); }

        inline void setAssets(const GizmosAssets& assets) { _assets = assets; }

        void render(const glm::mat4& viewProjection);

    private:
        GizmosAssets _assets{};
        std::stack<glm::vec4> _colorStack;
        std::vector<GizmoRender> _renderList;

        void pushRender(const std::shared_ptr<OpenGLMesh>& mesh, const glm::mat4& model);
    };
}
