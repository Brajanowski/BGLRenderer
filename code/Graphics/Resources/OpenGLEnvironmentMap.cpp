#include "OpenGLEnvironmentMap.h"

namespace BGLRenderer
{
    OpenGLEnvironmentMap::OpenGLEnvironmentMap(const std::string& name) :
        _name(name)
    {
        // TODO - hardcoded resolutions
        _cubemap = std::make_shared<OpenGLCubemap>(name, 1024, GL_RGB32F);
        _irradianceMap = std::make_shared<OpenGLCubemap>(name, 1024, GL_RGB32F);
    }
}
