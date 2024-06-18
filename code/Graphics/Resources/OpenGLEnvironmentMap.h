#pragma once

#include "../OpenGLBase.h"
#include "OpenGLTexture2D.h"
#include "OpenGLCubemap.h"

namespace BGLRenderer
{
    class OpenGLEnvironmentMap
    {
    public:
        OpenGLEnvironmentMap(const std::string& name);

        inline const std::shared_ptr<OpenGLCubemap>& cubemap() const { return _cubemap; }
        inline const std::shared_ptr<OpenGLCubemap>& irradianceMap() const { return _irradianceMap; }

        inline const std::string& name() const { return _name; }

    private:
        std::string _name;
        std::shared_ptr<OpenGLCubemap> _cubemap;
        std::shared_ptr<OpenGLCubemap> _irradianceMap;
    };
}
