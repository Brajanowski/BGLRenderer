#pragma once

#include <glm.hpp>

#include <gtc/quaternion.hpp>
#include <gtc/quaternion.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <ext/matrix_clip_space.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>
#include <gtx/io.hpp>

#include <iostream>

namespace BGLRenderer
{
    static void printVec3(const glm::vec3& vec)
    {
        std::cout << vec << std::endl;
    }
    
    static void printVec4(const glm::vec4& vec)
    {
        std::cout << vec << std::endl;
    }
    
    static void printMatrix(const std::string& name, const glm::mat4& matrix)
    {
        std::cout << "Matrix " << name << std::endl;
        std::cout << matrix << std::endl;
    }
}

