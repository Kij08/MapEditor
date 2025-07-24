//
// Created by Kij on 25/06/18.
//

#include "Object.h"
#include "Components/MeshComponent.h"

Object::Object(Scene* s) : Empty(s)
{
    Transform t = { .position = glm::vec3(0, 0, 0), .rotation = glm::vec3(0, 0, 0), .scale = glm::vec3(0.5, 0.5, 0.5) };
    SetTransform(t);

    Ka = 0;
    Kd = 0;
    Ks = 0;

    meshComponent = std::make_unique<MeshComponent>(this, "../DefaultContent/Meshes/CH_Player_Ship.obj", "../DefaultContent/Textures/SpaceShip_CLR.png");
}

Object::Object(Scene *s, std::string name) : Empty(s, name) {
}

Object::Object(Scene *s, std::string name, Transform t) : Empty(s, name, t) {
}

Object::Object(Scene* s, std::string model, std::string tex) : Empty(s)
{
    Transform t = { .position = glm::vec3(0, 0, 0), .rotation = glm::vec3(0, 0, 0), .scale = glm::vec3(0.5, 0.5, 0.5) };
    SetTransform(t);

    Ka = 0;
    Kd = 0;
    Ks = 0;

    meshComponent = std::make_unique<MeshComponent>(this, model, tex);
}

Object::Object(Scene *s, Transform t, const MeshComponentDefinition& mc) : Empty(s, t) {
    meshComponent = std::make_unique<MeshComponent>(this, mc.meshPath, mc.texturePath);
}

void Object::Tick()
{

    //objTransform.rotation.x += 0.1;
    //objTransform.rotation.y += 0.5;
}

void Object::Begin() {

}

ObjectDefinition Object::GetObjectDefinition() {
    ObjectDefinition def { .objectName = ObjectDisplayName, .className = "Object", .transform = objTransform };
    MeshComponentDefinition* meshDef = new MeshComponentDefinition();
    meshDef->componentName = "MeshComp";
    meshDef->className = "MeshComponent";
    meshDef->componentTransform = meshComponent->GetComponentTransform();
    meshDef->meshPath = meshComponent->GetModelPath();
    meshDef->texturePath = meshComponent->GetTexturePath();
    def.ComponentDefinitions.push_back(meshDef);
    return def;
}
