//
// Created by Kij on 25/06/19.
//

#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include <string>
#include <unordered_map>
#include <memory>

struct TextureLoader {

};

struct ModelLoader {

};

class AssetManager {
    std::unordered_map<std::string, std::shared_ptr<class Texture>> LoadedTextures;
    std::unordered_map<std::string, std::shared_ptr<class Mesh>> LoadedMeshes;

    std::shared_ptr<Texture> LoadTexture(std::string texturePath);
    std::shared_ptr<Mesh> LoadMesh(std::string modelPath);

public:
    AssetManager() {};


    void LoadObject(class Object* obj);


};

#endif //ASSETLOADER_H
