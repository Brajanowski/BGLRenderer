#include "SceneObject.h"

namespace BGLRenderer
{
    SceneObject::SceneObject(const std::string& name) :
        _name(name)
    {
    }

    SceneObject::~SceneObject()
    {
    }

    void SceneObject::setSubmeshes(const std::vector<RenderObjectSubmesh>& submeshes)
    {
        _submeshes = submeshes;
    }
}
