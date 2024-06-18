cmake_minimum_required(VERSION 3.8)
project(BGLrenderer)

set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

# GLAD
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/glad)

# SDL
include(sdl)

# IMGUI
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/ImGui)

# glm
set(GLM_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/lib/glm)

# rapid json
set(RAPIDJSON_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/lib/rapidjson)

add_executable(BGLrenderer
        code/Foundation/Base.h
        code/Platform/EntryPoint.cpp
        code/Foundation/Engine.h
        code/Foundation/Engine.cpp
        code/Foundation/Timer.h
        code/Foundation/Application.h
        code/Platform/SDLWindow.h
        code/Platform/SDLWindow.cpp
        code/Foundation/Log.h
        code/Foundation/Log.cpp
        code/Foundation/Publisher.h
        code/Foundation/ConsoleWindow.h
        code/Foundation/ConsoleWindow.cpp
        code/Foundation/Input.h
        code/Foundation/Input.cpp
        code/Utility/RapidJSONParsers.h
        code/Utility/RapidJSONParsers.cpp
        code/Foundation/Config.h
        code/Foundation/Config.cpp
        code/Assets/ConfigLoader.h
        code/Assets/ConfigLoader.cpp
        code/Foundation/GLMMath.h
        code/World/Transform.h
        code/World/PerspectiveCamera.h
        code/Graphics/OpenGLBase.h
        code/Graphics/OpenGLBase.cpp
        code/Graphics/Resources/OpenGLProgram.h
        code/Graphics/Resources/OpenGLProgram.cpp
        code/Graphics/Resources/OpenGLMaterial.h
        code/Graphics/Resources/OpenGLMaterial.cpp
        code/Graphics/Resources/OpenGLTexture2D.h
        code/Graphics/Resources/OpenGLTexture2D.cpp
        code/Graphics/Resources/OpenGLMesh.h
        code/Graphics/Resources/OpenGLMesh.cpp
        code/Graphics/Resources/OpenGLCubemap.h
        code/Graphics/Resources/OpenGLCubemap.cpp
        code/Graphics/Resources/OpenGLEnvironmentMap.h
        code/Graphics/Resources/OpenGLEnvironmentMap.cpp
        code/Graphics/Resources/OpenGLFramebuffer.h
        code/Graphics/Resources/OpenGLFramebuffer.cpp
        code/Graphics/OpenGLRenderObject.h
        code/Graphics/OpenGLRenderer.h
        code/Graphics/OpenGLRenderer.cpp
        code/World/Scene.h
        code/World/Scene.cpp
        code/World/SceneObject.h
        code/World/SceneObject.cpp
        code/Assets/AssetContentLoader.h
        code/Assets/AssetContentLoader.cpp
        code/Assets/ConcreteAssetManager.h
        code/Assets/ConcreteAssetManager.cpp
        code/Assets/AssetManager.h
        code/Assets/AssetManager.cpp
        code/Assets/AssetFileChangesObserver.h
        code/Assets/AssetFileChangesObserver.cpp
        code/Assets/AssetManagerTypes.h
        code/Assets/ProgramLoader.h
        code/Assets/ProgramLoader.cpp
        code/Assets/TextureLoader.h
        code/Assets/TextureLoader.cpp
        code/Assets/ModelLoader.h
        code/Assets/ModelLoader.cpp
        code/Assets/MaterialLoader.h
        code/Assets/MaterialLoader.cpp
        code/Assets/SceneLoader.h
        code/Assets/SceneLoader.cpp
        code/Foundation/ObjectInMemoryCache.h
        code/Sandbox/ApplicationSandbox.h
        code/Sandbox/ApplicationSandbox.cpp
        code/Utility/stb_image.h
        code/Utility/cgltf.h
        lib/imgui/backends/imgui_impl_opengl3.cpp
        lib/imgui/backends/imgui_impl_opengl3.h
        lib/imgui/backends/imgui_impl_sdl.cpp
        lib/imgui/backends/imgui_impl_sdl.h
        code/Graphics/Gizmos.cpp
        code/Graphics/Gizmos.h
        code/Graphics/EnvironmentMapGenerator.cpp
        code/Graphics/EnvironmentMapGenerator.h
)

if (MSVC)
    target_compile_options(BGLrenderer PRIVATE /W4 /WX)
else ()
    target_compile_options(BGLrenderer PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif ()

target_compile_features(BGLrenderer PRIVATE cxx_std_20)

target_include_directories(BGLrenderer PUBLIC ./code/)
target_include_directories(BGLrenderer PRIVATE ${SDL2_INCLUDE_DIRS})

# SDL2
target_link_libraries(BGLrenderer ${SDL2_LIBRARIES})
target_include_directories(BGLrenderer PRIVATE ${SDL2_INCLUDE_DIRS})

# GLAD
target_link_libraries(BGLrenderer glad)
target_include_directories(BGLrenderer PRIVATE ${GLAD_INCLUDE_DIRS})

# IMGUI
target_link_libraries(BGLrenderer imgui)
target_include_directories(BGLrenderer PRIVATE ${IMGUI_INCLUDE_DIRS})

# glm
target_include_directories(BGLrenderer PRIVATE ${GLM_INCLUDE_DIRS})

# rapid json
target_include_directories(BGLrenderer PRIVATE ${RAPIDJSON_INCLUDE_DIRS})
