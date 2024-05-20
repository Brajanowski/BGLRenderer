#pragma once

#include "OpenGLBase.h"
#include "OpenGLMaterial.h"
#include "OpenGLMesh.h"

#include <vector>

namespace BGLRenderer
{
    struct RenderObjectSubmesh
    {
        std::shared_ptr<OpenGLMaterial> material;
        std::shared_ptr<OpenGLMesh> mesh;
    };
    
    class OpenGLRenderObject
    {
    public:
        OpenGLRenderObject() = default;
        ~OpenGLRenderObject() = default;

        inline void addSubmesh(const RenderObjectSubmesh& submesh)
        {
            _submeshes.push_back(submesh);
        }

        inline const std::vector<RenderObjectSubmesh>& submeshes() const { return _submeshes; }

    private:
        std::vector<RenderObjectSubmesh> _submeshes;
    };
}
