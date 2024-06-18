#pragma once

#include <Foundation/Base.h>

#include <Graphics/Resources/OpenGLProgram.h>
#include <Graphics/Resources/OpenGLTexture2D.h>
#include <Graphics/Resources/OpenGLMaterial.h>
#include <Graphics/OpenGLRenderObject.h>

#include <Foundation/ObjectInMemoryCache.h>

namespace BGLRenderer
{
    typedef std::shared_ptr<ObjectInMemoryCache<std::string, OpenGLProgram>> OpenGLProgramsCache;
    typedef std::shared_ptr<ObjectInMemoryCache<std::string, OpenGLTexture2D>> OpenGLTexture2DCache;
    typedef std::shared_ptr<ObjectInMemoryCache<std::string, OpenGLRenderObject>> OpenGLRenderObjectCache;
    typedef std::shared_ptr<ObjectInMemoryCache<std::string, OpenGLMaterial>> OpenGLMaterialCache;
}
