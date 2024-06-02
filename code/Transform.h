#pragma once

#include "GLMMath.h"

namespace BGLRenderer
{
    struct Transform
    {
        glm::vec3 position = glm::vec3(0, 0, 0);
        glm::vec3 scale = glm::vec3(1, 1, 1);
        glm::quat rotation = glm::identity<glm::quat>();

        inline glm::mat4x4 modelMatrix() const
        {
            return glm::mat4_cast(rotation) * glm::translate(glm::mat4x4(1.0f), position) * glm::scale(glm::mat4x4(1.0f), scale);
        }
    };
}
