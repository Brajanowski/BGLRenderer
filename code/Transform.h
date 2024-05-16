#pragma once

#include <glm.hpp>
#include <gtc/quaternion.hpp>

namespace BGLRenderer
{
    struct Transform
    {
        glm::vec3 position = glm::vec3(0, 0, 0);
        glm::vec3 scale = glm::vec3(1, 1, 1);
        glm::quat rotation = glm::quat(0, 0, 0, 1);

        inline glm::mat4x4 modelMatrix() const
        {
            return glm::mat4_cast(rotation) * glm::scale(glm::mat4x4(1.0f), scale) * glm::translate(glm::mat4x4(1.0f), position);
        }
    };
}
