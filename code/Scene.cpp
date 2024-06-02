#include "Scene.h"

namespace BGLRenderer
{
    Scene::Scene(const std::string& name) :
        _name(name)
    {
    }

    Scene::~Scene()
    {
    }

    std::shared_ptr<SceneObject> Scene::createSceneObject(const std::string& name)
    {
        std::shared_ptr<SceneObject> object = std::make_shared<SceneObject>(name);
        _sceneObjects.push_back(object);

        return object;
    }

    void Scene::clear()
    {
        _sceneObjects.clear();
    }
}
