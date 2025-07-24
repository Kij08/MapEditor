//
// Created by Kij on 25/07/22.
//

#ifndef ENGINEUI_H
#define ENGINEUI_H

#include "AssetLoader.h"
#include "FileManager.h"
#include "../../include/imgui/imgui_impl_glfw.h"
#include "../../include/imgui/imgui_impl_vulkan.h"
#include "../../include/imgui/imgui_internal.h"
#include "../../include/imgui/imgui.h"
#include "../Scene/Scene.h"

namespace EngineUI {
    struct UITexture {
        std::shared_ptr<Texture> texture; //ptr to texture information
        VkDescriptorSet DS; //The field that ImGui uses in their backend to keep track of images
    };
    inline AssetManager UIAssets;
    inline UITexture folderIcon;
    inline UITexture meshIcon;
    inline UITexture imageIcon;
    //Call once to load UI textures
    inline void LoadUIAssets() {
        folderIcon.texture = UIAssets.LoadTexture("../Engine/EngineContent/Icons/open-folder.png", ETextureType::UITexture);
        folderIcon.DS = ImGui_ImplVulkan_AddTexture(folderIcon.texture->GetAllocation().TextureSampler, folderIcon.texture->GetAllocation().TextureImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        meshIcon.texture = UIAssets.LoadTexture("../Engine/EngineContent/Icons/cube.png", ETextureType::UITexture);
        meshIcon.DS = ImGui_ImplVulkan_AddTexture(meshIcon.texture->GetAllocation().TextureSampler, meshIcon.texture->GetAllocation().TextureImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        imageIcon.texture = UIAssets.LoadTexture("../Engine/EngineContent/Icons/files.png", ETextureType::UITexture);
        imageIcon.DS = ImGui_ImplVulkan_AddTexture(imageIcon.texture->GetAllocation().TextureSampler, imageIcon.texture->GetAllocation().TextureImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    //Flags representing whether to show a certain window in the ui dock space or not
    enum class WindowDisplayFlagValues {
        ShowDetails = 1 << 0,
        ShowSpawn = 1 << 1,
        ShowSceneTree = 1 << 2,
        ShowFileExplorer = 1 << 3,
    };

    struct WindowDisplayBit {
        uint8_t Flags = 0b11111111; //Enable all windows by default

        void SetFlag(WindowDisplayFlagValues flag) {
            Flags |= static_cast<int>(flag);
        }

        void FlipFlag(WindowDisplayFlagValues f) {
            Flags ^= static_cast<int>(f);
        }

        bool HasFlag(WindowDisplayFlagValues f) {
            return (Flags & static_cast<int>(f)) == static_cast<int>(f);
        }
    };

    //Define create new map popup modal
    //Must be defined wherever you want to call open modal due to ImGui stack restriction
    inline void CreateMapModal(bool& bIsMapLoaded, Scene*& s) {
        if (ImGui::BeginPopupModal("New Map Creation", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
            ImGui::Text("New Map Creation");
            static std::string mapName;
            static std::string mapPath;
            ImGui::InputText("Map Name", &mapName);
            if (mapName.empty()) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                ImGui::Text("Map Name cannot be empty.");
                ImGui::PopStyleColor();
            }
            ImGui::InputText("Map Path", &mapPath);
            ImGui::SameLine();
            if (ImGui::Button("Browse##Map")) {
                mapPath = FileManager::OpenChooseDirectoryDialog();
            }
            if (FileManager::ValidateFilePath(mapPath) == false) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                ImGui::Text("Map path invalid");
                ImGui::PopStyleColor();
            }

            if (ImGui::Button("Cancel##Map")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Create##Map")) {
                //Only create a map if the name and path are valid
                if (mapName.empty() == false && FileManager::ValidateFilePath(mapPath)) {
                    ImGui::CloseCurrentPopup();
                    bIsMapLoaded = FileManager::CreateNewMap(mapPath, mapName, s);
                }
            }

            ImGui::EndPopup();
        }
    }

    inline void StartupMenu(bool& bIsMapLoaded, Scene*& s) {
        ImGuiWindowFlags MapActionFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;
        ImVec2 centre = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(180, 120), ImGuiCond_Appearing);
        if (ImGui::Begin("Map Actions", nullptr, MapActionFlags)) {
            if (ImGui::Button("New Map")) {
                ImGui::OpenPopup("New Map Creation");
            }

            if (ImGui::Button("Open Map")) {
                bIsMapLoaded = FileManager::OpenMapFile(s);
            }

            CreateMapModal(bIsMapLoaded, s);

            ImGui::End();
        }

    }

    //Render buttons displayed in the top of the centre viewport to control things like the state of the viewer and the object edit gizmo
    inline void ViewportButtons(ImGuiID dockSpaceID, Scene* s) {
        //Set button pos to always be in the top left of the centre viewport
        ImVec2 topLeftViewport = ImGui::DockBuilderGetCentralNode(dockSpaceID)->Pos;
        ImGui::SetNextWindowPos(topLeftViewport, ImGuiCond_Always);
        ImGuiWindowFlags mainViewportBarFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize;
        if (ImGui::Begin("ViewportButtons", nullptr, mainViewportBarFlags)) {
            if (ImGui::Button(to_string(s->GetViewer()->GetState()).c_str(), ImVec2(80, 20))) {
                s->GetViewer()->EnumerateState();
            }
            ImGui::End();
        }
    }

    //Display main menu bar
    inline void MainMenuBar(bool& bIsMapLoaded, Scene* s, WindowDisplayBit& displayFlags) {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New..", "Ctrl+N")) {
                    ImGui::OpenPopup("New Map Creation");
                }
                if (ImGui::MenuItem("Open..", "Ctrl+O")) {
                    bIsMapLoaded = FileManager::OpenMapFile(s);
                    CreateMapModal(bIsMapLoaded, s);
                }
                if (ImGui::MenuItem("Save", "Ctrl+S"))   { /* Do stuff */ }
                if (ImGui::MenuItem("Close", "Ctrl+W"))  {  }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Settings")) { /* Do stuff */ }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Windows"))
            {
                if (ImGui::MenuItem("Details Panel")) {
                    displayFlags.FlipFlag(WindowDisplayFlagValues::ShowDetails);
                }
                if (ImGui::MenuItem("Spawn Panel")) {
                    displayFlags.FlipFlag(WindowDisplayFlagValues::ShowSpawn);
                }
                if (ImGui::MenuItem("Scene Tree Panel")) {
                    displayFlags.FlipFlag(WindowDisplayFlagValues::ShowSceneTree);
                }
                if (ImGui::MenuItem("File Explorer")) {
                    displayFlags.FlipFlag(WindowDisplayFlagValues::ShowFileExplorer);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    inline void SceneTreePanel(SceneNode*& selectedNode, Scene* s) {
        ImGui::Begin("Scene Tree");
        //Begin recursive tree render
        SceneNode* newNode = s->GetSceneRoot()->AddUITreeNode();
        if (newNode != nullptr) {
            //There is a new selected node
            if (selectedNode) {
                //Check if selected node is not nullptr incase there is no previous selection
                selectedNode->thisObject->isSelectedInScene = false;
            }
            selectedNode = newNode;
        }
        ImGui::End();
    }

    //Show details panel for selected object
    inline void DetailsPanel(SceneNode*& selectedNode, Scene* s) {
        ImGui::Begin("Object Info Panel");
        //If there is a selected object show its details

        if (selectedNode) {

            ImGui::Text("Object Transform");

            float pos[3] = {selectedNode->thisObject->GetTransform().position.x,
                selectedNode->thisObject->GetTransform().position.y,
                selectedNode->thisObject->GetTransform().position.z
            };

            float rot[3] = {selectedNode->thisObject->GetTransform().rotation.x,
                selectedNode->thisObject->GetTransform().rotation.y,
                selectedNode->thisObject->GetTransform().rotation.z
            };

            float scale[3] = {selectedNode->thisObject->GetTransform().scale.x,
                selectedNode->thisObject->GetTransform().scale.y,
                selectedNode->thisObject->GetTransform().scale.z
            };

            ImGui::InputFloat3("Position", pos);
            ImGui::InputFloat3("Rotation", rot);
            ImGui::InputFloat3("Scale", scale);

            Transform t = {glm::vec3{pos[0], pos[1], pos[2]}, glm::vec3{rot[0], rot[1], rot[2]}, glm::vec3{scale[0], scale[1], scale[2]}};

            selectedNode->thisObject->SetTransform(t);

            //Loop through all object components and call their definition of render component UI
            for (auto c : selectedNode->thisObject->GetComponents()) {
                c->RenderComponentImGui(s);
            }

            //If we click in the viewport on nothing then unfocus the object
            auto io = ImGui::GetIO();
            if (!io.WantCaptureMouse && io.MouseDown[0]) {
                //Clear selected node
                selectedNode->thisObject->isSelectedInScene = false;
                selectedNode = nullptr;
            }
        }
        ImGui::End();
    }

    inline void SpawnPanel(bool& bIsMapLoaded, Scene* s) {
        ImGui::Begin("Spawn Panel");
        if (bIsMapLoaded) {
            if (ImGui::Button("Spawn Spaceship")) {
                s->CreateObject<Object>();
            }
            if (ImGui::Button("Spawn small asteroid")) {
                s->CreateObject<Object>("../DefaultContent/Meshes/SM_Asteroid_SML_A.obj", "../DefaultContent/Textures/Asteroid_SML_CLR.png");
            }
        }
        ImGui::End();
    }

    inline void FileExplorer(Scene* s) {
        ImGui::Begin("File Explorer");
        if (ImGui::GetIO().MouseClicked[3]) {
            FileManager::SetNewCurrentDirectory(FileManager::currentWorkingDirectory.parent_path());
        }

        ImGuiStyle& style = ImGui::GetStyle();
        int fileCount = FileManager::DirectoryFiles.size();
        float window_visible_x2 = ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x;
        for (int n = 0; n < fileCount; n++) {
            auto& file = FileManager::DirectoryFiles[n];

            ImGui::PushID(n);
            static bool selected = false;
            if (file.extension() == ".obj") {
                ImGui::BeginGroup();
                ImGui::BeginGroup();
                ImGui::SetNextItemAllowOverlap();
                ImGui::Selectable("##FileSelect", &selected, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(110, 110));
                if (ImGui::IsMouseDoubleClicked(0))
                    selected = !selected;
                ImGui::Image(meshIcon.DS, ImVec2(100, 100));
                ImGui::Text(file.filename().c_str());
                ImGui::EndGroup();
                ImGui::EndGroup();
            }
            else if (file.extension() == ".png") {
                ImGui::BeginGroup();
                ImGui::BeginGroup();
                ImGui::SetNextItemAllowOverlap();
                ImGui::Selectable("##FileSelect", &selected, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(110, 110));
                if (ImGui::IsMouseDoubleClicked(0))
                    selected = !selected;
                ImGui::Image(imageIcon.DS, ImVec2(100, 100));
                ImGui::Text(file.filename().c_str());
                ImGui::EndGroup();
                ImGui::EndGroup();
            }
            else {
                ImGui::BeginGroup();
                ImGui::BeginGroup();
                ImGui::Selectable("##FileSelect", &selected, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(110, 110));
                if (ImGui::IsMouseDoubleClicked(0))
                    selected = !selected;
                ImGui::Image(folderIcon.DS, ImVec2(100, 100));
                ImGui::Text(file.string().substr(file.string().find_last_of('/')+1).c_str());
                ImGui::EndGroup();
                ImGui::EndGroup();

                if (selected) {
                    FileManager::SetNewCurrentDirectory(file);
                    selected = false;
                    ImGui::PopID();
                    break;
                }
            }
            float last_icon_x2 = ImGui::GetItemRectMax().x;
            float next_icon_x2 = last_icon_x2 + style.ItemSpacing.x + 100; // Expected position if next button was on same line
            if (n + 1 < fileCount && next_icon_x2 < window_visible_x2)
                ImGui::SameLine();
            ImGui::PopID();
        }

        ImGui::End();
    }


    //Main function to render editor ui elements
    inline ImDrawData* RenderImGuiElements(Scene*& s) {
        static SceneNode* selectedNode = nullptr;
        static bool bIsMapLoaded = false;

        //Flags to hold the display state of each ui window
        static WindowDisplayBit displayFlags;

        // Start the Dear ImGui frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
        ImGui::ShowDemoWindow();

        //Create dockspace
        ImGuiDockNodeFlags flags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGuiID dockSpaceID = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), flags);


        if (!bIsMapLoaded) {
            StartupMenu(bIsMapLoaded, s);
        }

        MainMenuBar(bIsMapLoaded, s, displayFlags);
        ViewportButtons(dockSpaceID, s);


        if (displayFlags.HasFlag(WindowDisplayFlagValues::ShowSceneTree)) {
            SceneTreePanel(selectedNode, s);
        }

        if (displayFlags.HasFlag(WindowDisplayFlagValues::ShowDetails)) {
            DetailsPanel(selectedNode, s);
        }

        if (displayFlags.HasFlag(WindowDisplayFlagValues::ShowSpawn)) {
            SpawnPanel(bIsMapLoaded, s);
        }

        if (displayFlags.HasFlag(WindowDisplayFlagValues::ShowFileExplorer)) {
            FileExplorer(s);
        }

        ImGui::Render();
        return ImGui::GetDrawData();
    }
}


#endif //ENGINEUI_H
