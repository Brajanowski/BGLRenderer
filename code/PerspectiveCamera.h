#pragma once

#include <ext/matrix_clip_space.hpp>

#include "Transform.h"

namespace BGLRenderer
{
    struct PerspectiveCamera
    {
        Transform transform;

        float fieldOfView = 60.0f;
        float nearZ = 0.1f;
        float farZ = 1024.0f;
        float aspectRatio = 16.0f / 9.0f;

        inline glm::mat4x4 projectionMatrix() const { return glm::perspective(glm::radians(fieldOfView), aspectRatio, nearZ, farZ); }
    };
}
