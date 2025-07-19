cmake_minimum_required(VERSION 3.31)
project(MapEditor)

set(CMAKE_CXX_STANDARD 20 CMAKE_EXE_LINKER_FLAGS -static CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s -O3")
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
        Engine/Renderer/RTypes.h
        Engine/Renderer/RTypes.cpp
        include/imgui/imconfig.h
        include/imgui/imgui.cpp
        include/imgui/imgui.h
        include/imgui/imgui_demo.cpp
        include/imgui/imgui_draw.cpp
        include/imgui/imgui_impl_glfw.cpp
        include/imgui/imgui_impl_glfw.h
        include/imgui/imgui_impl_vulkan.cpp
        include/imgui/imgui_impl_vulkan.h
        include/imgui/imgui_internal.h
        include/imgui/imgui_tables.cpp
        include/imgui/imgui_widgets.cpp
        include/imgui/imstb_rectpack.h
        include/imgui/imstb_textedit.h
        include/imgui/imstb_truetype.h
        include/imgui/imgui_stdlib.cpp
        include/imgui/imgui_stdlib.h
        Engine/Utils/InputManager.cpp
        Engine/Utils/InputManager.h
        Engine/Primitives/Empty.cpp
        Engine/Primitives/Empty.h
        Engine/Primitives/Components/Camera.cpp
        Engine/Primitives/Components/Camera.h
        Engine/Primitives/ObjectTypes.h
        Engine/Primitives/Components/Component.cpp
        Engine/Primitives/Components/Component.h
        Engine/Primitives/Components/Collider.cpp
        Engine/Primitives/Components/Collider.h
        Engine/Primitives/Components/MeshComponent.cpp
        Engine/Primitives/Components/MeshComponent.h
        Engine/Utils/FileManager.cpp
        Engine/Utils/FileManager.h)