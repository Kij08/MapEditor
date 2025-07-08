//
// Created by Kij on 25/06/18.
//

#include "Object.h"

Object::Object()
{
    Transform t = { .position = glm::vec3(0, 0, 0), .rotation = glm::vec3(0, 0, 0), .scale = glm::vec3(0.5, 0.5, 0.5) };
    SetTransform(t);
    PATH_TO_MODEL = "../DefaultContent/Meshes/CH_Player_Ship.obj";
    PATH_TO_TEXTURE = "../DefaultContent/Textures/SpaceShip_CLR.png";

    Ka = 0;
    Kd = 0;
    Ks = 0;
}

Object::Object(std::string model, std::string tex)
{
    Transform t = { .position = glm::vec3(0, 0, 0), .rotation = glm::vec3(0, 0, 0), .scale = glm::vec3(0.5, 0.5, 0.5) };
    SetTransform(t);
    PATH_TO_MODEL = model;
    PATH_TO_TEXTURE = tex;

    Ka = 0;
    Kd = 0;
    Ks = 0;
}

void Object::Tick()
{

    objTransform.rotation.x += 0.1;
    objTransform.rotation.y += 0.5;
}

void Object::Begin() {

}