#pragma once

#include <memory>
#include <string>

#include "OpenGLBase.h"
#include "OpenGLProgram.h"

namespace BGLRenderer
{
    class OpenGLMaterial
    {
    public:
        OpenGLMaterial(const std::shared_ptr<OpenGLProgram>& program);
        ~OpenGLMaterial();

        void bind();

    private:
        std::shared_ptr<OpenGLProgram> _program;
    };
}
