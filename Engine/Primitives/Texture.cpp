//
// Created by Kij on 25/06/18.
//

#include "Texture.h"
#include "../Utils/FileManager.h"

Texture::~Texture() {
    FileManager::WriteStringToLog("Destroying texture: " + TextureName);
}
