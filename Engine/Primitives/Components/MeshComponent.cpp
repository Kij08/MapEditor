//
// Created by Kij on 25/07/17.
//

#include "MeshComponent.h"

#include <iostream>
#include <bits/ostream.tcc>

#include "../../Scene/Scene.h"

MeshComponent::MeshComponent(class Empty *o, std::string modelPath, std::string texPath) : Component(o), pathToModel(modelPath), pathToTexture(texPath) {
    Owner->GetLevel()->GetSceneAssetManager()->LoadMeshComponent(this);
}

void MeshComponent::RenderComponentImGui(Scene* s) {
    ImGui::Text("Mesh Component");

    if (ImGui::BeginCombo("##Object Meshes", MeshRef->GetName().c_str())) {
        for (const auto& meshPair : s->GetSceneAssetManager()->GetLoadedMeshes()) {
            if (ImGui::Selectable(meshPair.second->GetName().c_str())) {
                MeshRef = meshPair.second;
            }
        }

        ImGui::EndCombo();
    }

    ImGui::Text("Mesh Texture");
    if (ImGui::BeginCombo("##Object Textures", TextureRef->GetName().c_str())) {

        for (const auto& texPair : s->GetSceneAssetManager()->GetLoadedTextures()) {
            if (ImGui::Selectable(texPair.second->GetName().c_str())) {
                TextureRef = texPair.second;
            }
        }

        ImGui::EndCombo();
    }
}
