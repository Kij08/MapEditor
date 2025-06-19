cmake_minimum_required(VERSION 3.31)
project(MapEditor)

set(CMAKE_CXX_STANDARD 20)
add_link_options(-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi)
include_directories(/include /imgui)

add_executable(MapEditor main.cpp
        Renderer.cpp Renderer.h
        Primitives/Mesh.cpp Primitives/Mesh.h
        include/stb_image.h include/tiny_obj_loader.h
        Primitives/Object.cpp Primitives/Object.h
        Engine/Scene/Scene.cpp
        Engine/Scene/Scene.h
        Engine/Scene/SceneViewer.cpp
        Engine/Scene/SceneViewer.h
        Primitives/Texture.cpp
        Primitives/Texture.h
        Primitives/Material.cpp
        Primitives/Material.h
        Engine/Utils/AssetLoader.cpp
        Engine/Utils/AssetLoader.h)