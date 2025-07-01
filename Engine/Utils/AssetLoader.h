//
// Created by Kij on 25/06/19.
//

#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include "../Renderer/Renderer.h"

struct TextureLoader {

};

struct ModelLoader {

};

class AssetManager {
    std::unordered_map<std::string, std::shared_ptr<Texture>> LoadedTextures;
    std::unordered_map<std::string, std::shared_ptr<class Mesh>> LoadedMeshes;

    std::shared_ptr<Texture> LoadTexture(std::string texturePath);
    std::shared_ptr<class Mesh> LoadMesh(std::string modelPath);

public:
    AssetManager();


    void LoadObject(Object* obj);


};

#endif //ASSETLOADER_H
