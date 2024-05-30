#pragma once

#include "Base.h"

#include "OpenGLProgram.h"
#include "OpenGLTexture2D.h"
#include "OpenGLRenderObject.h"

#include "ObjectInMemoryCache.h"

namespace BGLRenderer
{
    typedef std::shared_ptr<ObjectInMemoryCache<std::string, OpenGLProgram>> OpenGLProgramsCache;
    typedef std::shared_ptr<ObjectInMemoryCache<std::string, OpenGLTexture2D>> OpenGLTexture2DCache;
    typedef std::shared_ptr<ObjectInMemoryCache<std::string, OpenGLRenderObject>> OpenGLRenderObjectCache;
    typedef std::shared_ptr<ObjectInMemoryCache<std::string, OpenGLMaterial>> OpenGLMaterialCache;
}
