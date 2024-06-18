#pragma once

#include <../../lib/glm/glm.hpp>

#include <../../lib/glm/gtc/quaternion.hpp>
#include <../../lib/glm/gtc/quaternion.hpp>
#include <../../lib/glm/gtc/matrix_transform.hpp>
#include <../../lib/glm/gtc/type_ptr.hpp>

#include <../../lib/glm/ext/matrix_clip_space.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <../../lib/glm/gtx/string_cast.hpp>
#include <../../lib/glm/gtx/io.hpp>

namespace BGLRenderer::MathUtils
{
    inline glm::mat4 modelMatrix(glm::vec3 position, glm::vec3 scale, glm::quat rotation)
    {
        glm::mat4 translateMatrix = glm::translate(glm::mat4x4(1.0f), position);
        glm::mat4 scaleMatrix = glm::scale(glm::mat4x4(1.0f), scale);
        glm::mat4 rotationMatrix = glm::mat4_cast(rotation);

        return translateMatrix * scaleMatrix * rotationMatrix;
    }
}
