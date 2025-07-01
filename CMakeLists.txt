cmake_minimum_required(VERSION 3.31)
project(MapEditor)

set(CMAKE_CXX_STANDARD 20)
add_link_options(-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi)
include_directories(/include /include/imgui)

add_executable(MapEditor main.cpp
        Engine/Renderer/Renderer.cpp Engine/Renderer/Renderer.h
        Engine/Primitives/Mesh.cpp Engine/Primitives/Mesh.h
        include/stb_image.h include/tiny_obj_loader.h include/vk_mem_alloc.h
        Engine/Primitives/Object.cpp Engine/Primitives/Object.h
        Engine/Scene/Scene.cpp
        Engine/Scene/Scene.h
        Engine/Scene/SceneViewer.cpp
        Engine/Scene/SceneViewer.h
        Engine/Primitives/Texture.cpp
        Engine/Primitives/Texture.h
        Engine/Primitives/Material.cpp
        Engine/Primitives/Material.h
        Engine/Utils/AssetLoader.cpp
        Engine/Utils/AssetLoader.h
        Engine/Renderer/RDescriptors.cpp
        Engine/Renderer/RDescriptors.h
        Engine/Renderer/RTypes.h)