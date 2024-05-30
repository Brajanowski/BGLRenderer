cmake_minimum_required(VERSION 3.8)
project(BGLrenderer)

set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
set(WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

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
    code/Base.h
    code/EntryPoint.cpp
    code/Engine.h
    code/Engine.cpp
    code/Timer.h
    code/Application.h
    code/SDLWindow.h
    code/SDLWindow.cpp
    code/Log.h
    code/Log.cpp
    code/Publisher.h
    code/ConsoleWindow.h
    code/ConsoleWindow.cpp
    code/Input.h
    code/Input.cpp
    code/GLMMath.h
    code/Transform.h
    code/PerspectiveCamera.h
    code/OpenGLBase.h
    code/OpenGLBase.cpp
    code/OpenGLProgram.h
    code/OpenGLProgram.cpp
    code/OpenGLMaterial.h
    code/OpenGLMaterial.cpp
    code/OpenGLTexture2D.h
    code/OpenGLTexture2D.cpp
    code/OpenGLMesh.h
    code/OpenGLMesh.cpp
    code/OpenGLFramebuffer.h
    code/OpenGLFramebuffer.cpp
    code/OpenGLRenderObject.h
    code/OpenGLRenderer.h
    code/OpenGLRenderer.cpp
    code/AssetContentLoader.h
    code/AssetContentLoader.cpp
    code/ConcreteAssetManager.h
    code/ConcreteAssetManager.cpp
    code/AssetManager.h
    code/AssetManager.cpp
    code/AssetFileChangesObserver.h
    code/AssetFileChangesObserver.cpp
    code/AssetManagerTypes.h
    code/ProgramLoader.h
    code/ProgramLoader.cpp
    code/TextureLoader.h
    code/TextureLoader.cpp
    code/ModelLoader.h
    code/ModelLoader.cpp
    code/MaterialLoader.h
    code/MaterialLoader.cpp
    code/ObjectInMemoryCache.h
    code/ApplicationSandbox.h
    code/ApplicationSandbox.cpp
    code/stb_image.h
    code/cgltf.h
    lib/imgui/backends/imgui_impl_opengl3.cpp
    lib/imgui/backends/imgui_impl_opengl3.h
    lib/imgui/backends/imgui_impl_sdl.cpp
    lib/imgui/backends/imgui_impl_sdl.h
)

set_target_properties(BGLrenderer PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin")

if (MSVC)
    target_compile_options(BGLrenderer PRIVATE /W4 /WX)
else()
    target_compile_options(BGLrenderer PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

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
