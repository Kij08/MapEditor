//
// Created by Kij on 25/06/18.
//

#include "Object.h"

Object::Object(Scene* s) : Empty(s), PATH_TO_MODEL("../DefaultContent/Meshes/CH_Player_Ship.obj"), PATH_TO_TEXTURE("../DefaultContent/Textures/SpaceShip_CLR.png")
{
    Transform t = { .position = glm::vec3(0, 0, 0), .rotation = glm::vec3(0, 0, 0), .scale = glm::vec3(0.5, 0.5, 0.5) };
    SetTransform(t);

    Ka = 0;
    Kd = 0;
    Ks = 0;
}

Object::Object(Scene* s, std::string model, std::string tex) : Empty(s), PATH_TO_MODEL(model), PATH_TO_TEXTURE(tex)
{
    Transform t = { .position = glm::vec3(0, 0, 0), .rotation = glm::vec3(0, 0, 0), .scale = glm::vec3(0.5, 0.5, 0.5) };
    SetTransform(t);

    Ka = 0;
    Kd = 0;
    Ks = 0;
}

void Object::Tick()
{

    //objTransform.rotation.x += 0.1;
    //objTransform.rotation.y += 0.5;
}

void Object::Begin() {

}