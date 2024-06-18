#pragma once

#include "Transform.h"

namespace BGLRenderer
{
    struct PerspectiveCamera
    {
        Transform transform{};

        float fieldOfView = 60.0f;
        float nearZ = 0.1f;
        float farZ = 1024.0f;
        float aspectRatio = 16.0f / 9.0f;

        glm::vec3 forward() const
        {
            glm::mat3 rotationMatrix = glm::mat3_cast(transform.rotation);
            return -rotationMatrix[2];
        }

        glm::vec3 right() const
        {
            glm::mat3 rotationMatrix = glm::mat3_cast(transform.rotation);
            return rotationMatrix[0];
        }

        glm::vec3 up() const
        {
            glm::mat3 rotationMatrix = glm::mat3_cast(transform.rotation);
            return rotationMatrix[1];
        }

        inline glm::mat4x4 projection() const
        {
            //return glm::mat4(1.0f);
            return glm::perspective(glm::radians(fieldOfView), aspectRatio, nearZ, farZ);
        }
        
        inline glm::mat4x4 view() const
        {
            glm::mat4 rotationMatrix = glm::mat4_cast(glm::conjugate(transform.rotation));
            glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -transform.position);
            return rotationMatrix * translationMatrix;
        }

        inline glm::mat4x4 viewProjection() const { return projection() * view(); }
    };
}
