//
// Created by Kij on 25/06/18.
//

#include "Object.h"
#include "../Scene/Scene.h"

Object::Object()
{

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