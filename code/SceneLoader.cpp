#include "SceneLoader.h"

#include "RapidJSONParsers.h"

namespace BGLRenderer
{
    namespace MemberNames
    {
        static constexpr const char* SceneObjects = "scene_objects";

        static constexpr const char* SceneEntryModel = "model";
        static constexpr const char* SceneEntryMaterials = "materials";
        static constexpr const char* SceneEntryTransform = "transform";

        static constexpr const char* SceneEntryTransformPosition = "position";
        static constexpr const char* SceneEntryTransformScale = "scale";
        static constexpr const char* SceneEntryTransformRotationEulerAngles = "rotation_euler_angles";

        static constexpr const char* Name = "name";
    }

    SceneLoader::SceneLoader(const std::shared_ptr<AssetContentLoader>& assetContentLoader, const std::shared_ptr<ModelAssetManager>& modelAssetManager, const std::shared_ptr<MaterialAssetManager>& materialAssetManager) :
        _assetContentLoader(assetContentLoader),
        _modelAssetManager(modelAssetManager),
        _materialAssetManager(materialAssetManager)
    {
    }

    SceneLoader::~SceneLoader()
    {
    }

    std::shared_ptr<Scene> SceneLoader::load(const std::string& name)
    {
        _logger.debug("Loading scene: \"{}\"", name);

        std::shared_ptr<Scene> scene = std::make_shared<Scene>(name);

        if (!loadInto(scene, name))
        {
            _logger.error("Error occured while loading scene \"{}\"", name);
        }

        return scene;
    }

    bool SceneLoader::loadInto(const std::shared_ptr<Scene>& scene, const std::string& name)
    {
        ASSERT(scene != nullptr, "Cannot load scene asset into null scene");

        _logger.debug("Replacing scene \"{}\" with scene asset \"{}\"", scene->name(), name);

        scene->clear();

        std::vector<std::uint8_t> sceneAssetContent = _assetContentLoader->load(name);
        std::string assetContentString = std::string(sceneAssetContent.begin(), sceneAssetContent.end());

        rapidjson::Document document;
        document.Parse(assetContentString.c_str());

        if (!document.IsObject())
        {
            _logger.error("Document is not an object! Returning empty scene...");
            return false;
        }

        if (!checkDocumentForAssetType(document, "scene"))
        {
            return false;
        }

        if (document.HasMember(MemberNames::SceneObjects))
        {
            const rapidjson::Value& sceneObjects = document[MemberNames::SceneObjects];

            if (sceneObjects.IsArray())
            {
                for (rapidjson::SizeType i = 0; i < sceneObjects.Size(); ++i)
                {
                    const rapidjson::Value& entry = sceneObjects[i];

                    if (!entry.IsObject())
                    {
                        _logger.error("All entries of array \"{}\" must be an object, failed at {}", MemberNames::SceneObjects, i);
                        continue;
                    }

                    if (!loadSceneObject(entry, scene))
                    {
                        _logger.error("Failed at parsing scene object entry, index {}", i);
                    }
                }
            }
            else
            {
                _logger.error("\"{}\" must be an array", MemberNames::SceneObjects);
            }
        }

        return true;
    }

    bool SceneLoader::loadSceneObject(const rapidjson::Value& entry, const std::shared_ptr<Scene>& toScene)
    {
        if (!entry.IsObject())
        {
            _logger.error("Entry is not an object");
            return false;
        }

        if (!entry.HasMember(MemberNames::Name) || !entry[MemberNames::Name].IsString())
        {
            _logger.error("Entry doesn't have \"{}\" string value", MemberNames::Name);
            return false;
        }

        std::string sceneObjectName = getMemberValue<std::string>(entry[MemberNames::Name]);
        std::shared_ptr<SceneObject> sceneObject = toScene->createSceneObject(sceneObjectName);

        if (entry.HasMember(MemberNames::SceneEntryTransform))
        {
            const rapidjson::Value& transformValue = entry[MemberNames::SceneEntryTransform];

            if (transformValue.HasMember(MemberNames::SceneEntryTransformPosition))
            {
                sceneObject->transform().position = getMemberValue<glm::vec3>(transformValue[MemberNames::SceneEntryTransformPosition]);
            }

            if (transformValue.HasMember(MemberNames::SceneEntryTransformScale))
            {
                sceneObject->transform().scale = getMemberValue<glm::vec3>(transformValue[MemberNames::SceneEntryTransformScale]);
            }

            if (transformValue.HasMember(MemberNames::SceneEntryTransformRotationEulerAngles))
            {
                glm::vec3 eulerAngles = getMemberValue<glm::vec3>(transformValue[MemberNames::SceneEntryTransformRotationEulerAngles]);
                eulerAngles.x = glm::radians(eulerAngles.x);
                eulerAngles.y = glm::radians(eulerAngles.y);
                eulerAngles.z = glm::radians(eulerAngles.z);

                sceneObject->transform().rotation = glm::quat(eulerAngles);
            }
        }

        if (entry.HasMember(MemberNames::SceneEntryModel) && entry[MemberNames::SceneEntryModel].IsString())
        {
            std::string modelName = getMemberValue<std::string>(entry[MemberNames::SceneEntryModel]);
            std::shared_ptr<OpenGLRenderObject> renderObject = _modelAssetManager->get(modelName);

            if (renderObject == nullptr)
            {
                _logger.error("Couldn't load model: \"{}\", set in \"{}\"", modelName, sceneObjectName);
                return false;
            }

            if (entry.HasMember(MemberNames::SceneEntryMaterials))
            {
                std::vector<std::string> materialsNames;
                if (!getStringValuesFromArray(entry[MemberNames::SceneEntryMaterials], materialsNames))
                {
                    return false;
                }

                std::vector<RenderObjectSubmesh> submeshes;
                submeshes.resize(renderObject->submeshes().size());

                if (submeshes.size() < materialsNames.size())
                {
                    _logger.error("There is too much materials set for scene object: \"{}\", model has {} submeshes", sceneObjectName, submeshes.size());
                }
                else if (submeshes.size() > materialsNames.size())
                {
                    _logger.error("There is not enough materials set for scene object: \"{}\", model has {} submeshes", sceneObjectName, submeshes.size());
                }

                for (std::size_t submeshIndex = 0; submeshIndex < submeshes.size(); ++submeshIndex)
                {
                    std::shared_ptr<OpenGLMaterial> material;

                    if (submeshIndex < materialsNames.size())
                    {
                        material = _materialAssetManager->get(materialsNames[submeshIndex]);
                    }
                    else
                    {
                        material = renderObject->submeshes()[submeshIndex].material;
                    }

                    submeshes[submeshIndex] = {material, renderObject->submeshes()[submeshIndex].mesh};
                }

                sceneObject->setSubmeshes(submeshes);
            }
            else
            {
                sceneObject->setSubmeshes(renderObject->submeshes());
            }
        }

        return true;
    }
}
