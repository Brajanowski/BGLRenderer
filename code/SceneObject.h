#pragma once

#include "Base.h"
#include <string>

#include "Transform.h"
#include "OpenGLRenderObject.h"

namespace BGLRenderer
{
    class SceneObject
    {
    public:
        SceneObject(const std::string& name);
        ~SceneObject();

        void setSubmeshes(const std::vector<RenderObjectSubmesh>& submeshes);
        inline const std::vector<RenderObjectSubmesh>& submeshes() { return _submeshes; }

        inline Transform& transform() { return _transform; }
        inline const Transform& transform() const { return _transform; }

        inline const std::string& name() const { return _name; }
        inline void setName(const std::string& name) { _name = name; }

    private:
        std::string _name;
        Transform _transform{};
        
        std::vector<RenderObjectSubmesh> _submeshes;
    };
}
