//
// Created by Kij on 25/06/18.
//

#include "Object.h"
#include "../Engine/Scene/Scene.h"

Object::Object()
{
    modelVertexStartOffset = -1;
    modelVertexSizeInBuffer = -1;
    modelIndexStartOffset = -1;
    modelIndexSizeInBuffer = -1;

}

Object::Object(Transform t, std::string model, std::string tex)
{
    SetTransform(t);
    PATH_TO_MODEL = model;
    PATH_TO_TEXTURE = tex;

    modelVertexStartOffset = -1;
    modelVertexSizeInBuffer = -1;
    modelIndexStartOffset = -1;
    modelIndexSizeInBuffer = -1;

    Ka = 0;
    Kd = 0;
    Ks = 0;
}

void Object::Tick()
{
}