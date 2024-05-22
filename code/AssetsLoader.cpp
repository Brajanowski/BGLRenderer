#include "AssetsLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"
#include "Timer.h"

namespace BGLRenderer
{
    AssetsLoader::AssetsLoader(const std::shared_ptr<AssetContentLoader>& contentLoader) :
        _contentLoader(contentLoader)
    {
        //stbi_set_flip_vertically_on_load(true);
    }

    void AssetsLoader::registerTexture(const std::string& name, const std::shared_ptr<OpenGLTexture2D>& texture)
    {
        if (isTextureLoaded(name))
        {
            _logger.error("Texture with given name \"{}\" is already registered!", name);
            return;
        }

        _loadedTextures[name] = texture;
    }

    std::shared_ptr<OpenGLProgram> AssetsLoader::loadProgram(const std::string& name)
    {
        return loadProgram(name + ".vert", name + ".frag");
    }

    std::shared_ptr<OpenGLProgram> AssetsLoader::loadProgram(const std::string& vertexShaderName, const std::string& fragmentShaderName)
    {
        std::string programName = vertexShaderName + "+" + fragmentShaderName;

        if (_loadedPrograms.contains(programName))
        {
            return _loadedPrograms[programName];
        }

        std::vector<std::uint8_t> vertexContent = _contentLoader->load(vertexShaderName);
        std::vector<std::uint8_t> fragmentContent = _contentLoader->load(fragmentShaderName);

        std::shared_ptr<OpenGLProgram> program = std::make_shared<OpenGLProgram>(std::string(vertexContent.begin(), vertexContent.end()), std::string(fragmentContent.begin(), fragmentContent.end()));
        _loadedPrograms[programName] = program;
        return program;
    }

    std::shared_ptr<OpenGLTexture2D> AssetsLoader::loadTexture(const std::string& name)
    {
        if (isTextureLoaded(name))
        {
            return _loadedTextures[name];
        }

        _logger.debug("Loading texture from: {}", name);

        std::vector<std::uint8_t> textureFileContent = _contentLoader->load(name);
        std::shared_ptr<OpenGLTexture2D> texture = loadTextureFromImageData(textureFileContent);
        registerTexture(name, texture);
        return texture;
    }

    std::shared_ptr<OpenGLRenderObject> AssetsLoader::loadModel(const std::string& name, const std::shared_ptr<OpenGLProgram>& program)
    {
        HighResolutionTimer loadingTimer;

        std::string path = ("assets/" + name);
        std::string basePath = name.find('/') == std::string::npos ? name : name.substr(0, name.find_last_of('/') + 1);

        _logger.debug("Loading model: {}, base path: {}", path, basePath);

        cgltf_options options{};
        cgltf_data* data = nullptr;
        cgltf_result result = cgltf_parse_file(&options, path.c_str(), &data);
        ASSERT(result == cgltf_result_success, "Failed to load model!");

        result = cgltf_load_buffers(&options, data, path.c_str());
        ASSERT(result == cgltf_result_success, "Failed to load buffers!");

        result = cgltf_validate(data);
        ASSERT(result == cgltf_result_success, "Failed to validate data!");

        std::shared_ptr<OpenGLRenderObject> renderObject = std::make_shared<OpenGLRenderObject>();

        for (cgltf_size meshIndex = 0; meshIndex < data->meshes_count; ++meshIndex)
        {
            cgltf_mesh* mesh = &data->meshes[meshIndex];

            for (cgltf_size primitiveIndex = 0; primitiveIndex < mesh->primitives_count; ++primitiveIndex)
            {
                cgltf_primitive* primitive = &mesh->primitives[primitiveIndex];
                ASSERT(primitive->type == cgltf_primitive_type_triangles, "Primivite type must be triangles!");

                std::vector<GLfloat> positions;
                std::vector<GLfloat> normals;
                std::vector<GLfloat> tangents;
                std::vector<GLfloat> uvs0;
                std::vector<GLuint> indices;
                indices.reserve(primitive->indices->count);

                std::shared_ptr<OpenGLMaterial> openGLMaterial = std::make_shared<OpenGLMaterial>(program);
                if (primitive->material != nullptr)
                {
                    loadMaterialFromCGLTFMaterial(name, openGLMaterial, basePath, primitive->material);
                }

                for (cgltf_size attributeIndex = 0; attributeIndex < primitive->attributes_count; ++attributeIndex)
                {
                    cgltf_attribute* attribute = &primitive->attributes[attributeIndex];

                    if (attribute->type == cgltf_attribute_type_position)
                    {
                        loadAttributeDataIntoVector(positions, attribute, 3);
                    }
                    else if (attribute->type == cgltf_attribute_type_normal)
                    {
                        loadAttributeDataIntoVector(normals, attribute, 3);
                    }
                    else if (attribute->type == cgltf_attribute_type_tangent)
                    {
                        loadTangentAttributeDataIntoVector(tangents, attribute);
                    }
                    else if (attribute->type == cgltf_attribute_type_texcoord)
                    {
                        loadAttributeDataIntoVector(uvs0, attribute, 2);
                    }
                }

                //_logger.debug("Reading indices: {}", primitive->indices->count);
                for (cgltf_size indicesBufferIndex = 0; indicesBufferIndex < primitive->indices->count; ++indicesBufferIndex)
                {
                    cgltf_size index = cgltf_accessor_read_index(primitive->indices, indicesBufferIndex);
                    indices.push_back(static_cast<GLuint>(index));
                }

                std::shared_ptr<OpenGLMesh> openGLMesh = std::make_shared<OpenGLMesh>();
                openGLMesh->setVertices(positions.data(), positions.size());

                if (normals.empty())
                {
                    OpenGLMesh::calculateNormals(normals, positions, indices);
                }
                openGLMesh->setNormals(normals.data(), normals.size());

                if (uvs0.empty())
                {
                    uvs0.resize(positions.size() / 3 * 2, 0.0f);
                }
                openGLMesh->setUVs0(uvs0.data(), uvs0.size());

                if (tangents.empty())
                {
                    OpenGLMesh::calculateTangents(tangents, positions, uvs0, indices);
                }
                openGLMesh->setTangents(tangents.data(), tangents.size());

                openGLMesh->setIndices(indices.data(), indices.size());

                RenderObjectSubmesh submesh;
                submesh.material = openGLMaterial;
                submesh.mesh = openGLMesh;
                renderObject->addSubmesh(submesh);
            }
        }

        cgltf_free(data);

        _logger.debug("Mesh loaded in {}s", loadingTimer.elapsedSeconds());
        return renderObject;
    }

    std::shared_ptr<OpenGLTexture2D> AssetsLoader::loadTextureFromImageData(const std::uint8_t* bytes, size_t size)
    {
        ASSERT(bytes != nullptr, "Bytes is nullptr");
        ASSERT(size > 0, "Invalid size");

        int width;
        int height;
        int components;
        float* imageData = stbi_loadf_from_memory(bytes, size, &width, &height, &components, 0);
        ASSERT(imageData != nullptr, "Failed to load image data");

        GLenum internalFormat;
        GLenum dataFormat = GL_RGBA;

        bool gammaCorrection = false;

        if (components == 1)
        {
            internalFormat = dataFormat = GL_RED;
        }
        else if (components == 3)
        {
            internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (components == 4)
        {
            internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        std::shared_ptr<OpenGLTexture2D> texture = std::make_shared<OpenGLTexture2D>(width, height, internalFormat);
        texture->setPixels(dataFormat, (GLfloat*)imageData);

        stbi_image_free(imageData);
        return texture;
    }

    std::shared_ptr<OpenGLTexture2D> AssetsLoader::loadTextureFromImageData(const std::vector<std::uint8_t>& imageFileContent)
    {
        return loadTextureFromImageData(imageFileContent.data(), imageFileContent.size());
    }

    void AssetsLoader::loadMaterialFromCGLTFMaterial(const std::string& modelName, const std::shared_ptr<OpenGLMaterial>& target, const std::string& basePath, const cgltf_material* material)
    {
        if (material->pbr_metallic_roughness.base_color_texture.texture != nullptr)
        {
            loadCGLTFMaterialTextureToSlot(modelName, target, "baseColor", basePath, material->pbr_metallic_roughness.base_color_texture.texture);
        }

        if (material->emissive_texture.texture != nullptr)
        {
            loadCGLTFMaterialTextureToSlot(modelName, target, "emissive", basePath, material->emissive_texture.texture);
        }

        if (material->normal_texture.texture != nullptr)
        {
            loadCGLTFMaterialTextureToSlot(modelName, target, "normalMap", basePath, material->normal_texture.texture);
        }
    }

    void AssetsLoader::loadCGLTFMaterialTextureToSlot(const std::string& modelName, const std::shared_ptr<OpenGLMaterial>& target, const std::string& slotName, const std::string& basePath, const cgltf_texture* texture)
    {
        std::shared_ptr<OpenGLTexture2D> openGLTexture = nullptr;

        if (texture->image->uri != nullptr && texture->image->uri[0] != '\0')
        {
            std::string uri = texture->image->uri;
            std::string texturePath = basePath + uri;

            openGLTexture = loadTexture(texturePath);
        }
        else
        {
            std::string textureName = modelName + "+" + texture->image->name;

            if (isTextureLoaded(textureName))
            {
                openGLTexture = _loadedTextures[textureName];
            }
            else
            {
                const uint8_t* imageData = cgltf_buffer_view_data(texture->image->buffer_view);
                openGLTexture = loadTextureFromImageData(imageData, texture->image->buffer_view->buffer->size);
                registerTexture(textureName, openGLTexture);
            }
        }

        ASSERT(openGLTexture != nullptr, "Texture is not loaded!");

        // TODO: filter mode support
        if (texture->sampler != nullptr)
        {
            openGLTexture->setWrapMode(texture->sampler->wrap_s);
        }
        else
        {
            openGLTexture->setWrapMode(GL_REPEAT);
        }

        target->setTexture2D(slotName, openGLTexture);
    }

    void AssetsLoader::loadAttributeDataIntoVector(std::vector<GLfloat>& data, const cgltf_attribute* attribute, int components)
    {
        ASSERT(components > 0 && components <= 4, "Components must be in range 1-4");

        data.clear();
        data.reserve(attribute->data->count * components);

        for (cgltf_size bufferIndex = 0; bufferIndex < attribute->data->count; ++bufferIndex)
        {
            cgltf_float v[4] = {0, 0, 0, 0};
            if (!cgltf_accessor_read_float(attribute->data, bufferIndex, v, components))
            {
                _logger.error("Couldn't read buffer data, attribute name: {}, index: {}", attribute->name, bufferIndex);
                break;
            }

            for (int i = 0; i < components; ++i)
            {
                data.push_back(v[i]);
            }
        }
    }

    void AssetsLoader::loadTangentAttributeDataIntoVector(std::vector<GLfloat>& data, const cgltf_attribute* attribute)
    {
        data.clear();
        data.reserve(attribute->data->count * 3);

        for (cgltf_size bufferIndex = 0; bufferIndex < attribute->data->count; ++bufferIndex)
        {
            cgltf_float v[4] = {0, 0, 0, 0};
            if (!cgltf_accessor_read_float(attribute->data, bufferIndex, v, 4))
            {
                _logger.error("Couldn't read buffer data, attribute name: {}, index: {}", attribute->name, bufferIndex);
                break;
            }

            glm::vec3 tangent = {v[0], v[1], v[2]};
            tangent *= v[3];

            data.push_back(tangent.x);
            data.push_back(tangent.y);
            data.push_back(tangent.z);
        }
    }
}
