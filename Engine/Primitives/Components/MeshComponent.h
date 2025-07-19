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
    explicit MeshComponent(class Empty* o) : Component(o) {}
    void RenderComponentImGui(Scene* s) override;

    void SetMesh(std::shared_ptr<Mesh> mesh) { MeshRef = mesh; };
    Mesh* GetMesh() { return MeshRef.get(); }
    void SetTexture(std::shared_ptr<Texture> texture) { TextureRef = texture; };
    Texture* GetTexture() { return TextureRef.get(); }
private:
    std::shared_ptr<Mesh> MeshRef;
    std::shared_ptr<Texture> TextureRef;
};



#endif //MESHCOMPONENT_H
