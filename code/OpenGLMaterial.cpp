# include "OpenGLMaterial.h"

namespace BGLRenderer
{
    OpenGLMaterial::OpenGLMaterial(const std::shared_ptr<OpenGLProgram> &program) :
        _program(program)
    {
    }

    OpenGLMaterial::~OpenGLMaterial()
    {
    }

    void OpenGLMaterial::bind()
    {
        _program->bind();
    }
}
