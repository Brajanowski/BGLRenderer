#pragma once

#include <Foundation/GLMMath.h>

namespace BGLRenderer
{
    struct Transform
    {
        glm::vec3 position = glm::vec3(0, 0, 0);
        glm::vec3 scale = glm::vec3(1, 1, 1);
        glm::quat rotation = glm::identity<glm::quat>();

        inline glm::mat4x4 modelMatrix() const
        {
            return MathUtils::modelMatrix(position, scale, rotation);
        }
    };
}
