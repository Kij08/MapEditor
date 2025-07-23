//
// Created by Kij on 25/07/17.
//

#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H

#include "Component.h"
#include <memory>
#include "../Mesh.h"
#include "../Texture.h"

class MeshComponent : public Component{
public:
    MeshComponent(class Empty* o, std::string modelPath, std::string texPath);
    void RenderComponentImGui(Scene* s) override;

    void SetMesh(std::shared_ptr<Mesh> mesh) { MeshRef = std::move(mesh); }
    Mesh* GetMesh() { return MeshRef.get(); }
    void SetTexture(std::shared_ptr<Texture> texture) { TextureRef = std::move(texture); };
    Texture* GetTexture() { return TextureRef.get(); }

    const std::string& GetModelPath() { return pathToModel; }
    const std::string& GetTexturePath() { return pathToTexture; }

private:
    std::shared_ptr<Mesh> MeshRef;
    std::shared_ptr<Texture> TextureRef;

    std::string pathToModel;
    std::string pathToTexture;
};



#endif //MESHCOMPONENT_H
