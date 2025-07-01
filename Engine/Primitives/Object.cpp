//
// Created by Kij on 25/06/18.
//

#include "Object.h"
#include "../Scene/Scene.h"

Object::Object()
{
    Transform t = { .position = glm::vec3(0, 0, 0), .rotation = glm::vec3(0, 0, 0), .scale = glm::vec3(1, 1, 1) };
    SetTransform(t);
    PATH_TO_MODEL = "/DefaultContent/Meshes/CH_Player_Ship.obj";
    PATH_TO_TEXTURE = "/DefaultContent/Textures/SpaceShip_CLR.png";

    Ka = 0;
    Kd = 0;
    Ks = 0;
}

Object::Object(Transform t, std::string model, std::string tex)
{
    SetTransform(t);
    PATH_TO_MODEL = model;
    PATH_TO_TEXTURE = tex;

    Ka = 0;
    Kd = 0;
    Ks = 0;
}

void Object::Tick()
{
}