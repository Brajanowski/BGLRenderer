#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Foundation/Base.h>
#include "SceneObject.h"

namespace BGLRenderer
{
    class Scene
    {
    public:
        Scene(const std::string& name);
        ~Scene();

        std::shared_ptr<SceneObject> createSceneObject(const std::string& name);
        
        void clear();

        inline const std::string& name() const { return _name; }
        inline const std::vector<std::shared_ptr<SceneObject>>& objects() const { return _sceneObjects; }

    private:
        std::string _name;

        // HDR Handle
        std::vector<std::shared_ptr<SceneObject>> _sceneObjects;
    };
}
