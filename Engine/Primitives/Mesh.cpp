//
// Created by Kij on 25/06/18.
//

#include "Mesh.h"
#include "../Utils/FileManager.h"

Mesh::~Mesh() {
    FileManager::WriteStringToLog("Destroying mesh: " + MeshName);
}
