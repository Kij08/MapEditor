//
// Created by Kij on 25/06/19.
//

#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

struct TextureLoader {

};

struct ModelLoader {

};

class AssetManager {
    std::unordered_map<std::string, std::shared_ptr<class Texture>> LoadedTextures;
    std::unordered_map<std::string, std::shared_ptr<class Mesh>> LoadedMeshes;

    std::shared_ptr<Texture> LoadTexture(std::string texturePath);
    std::shared_ptr<Mesh> LoadMesh(std::string modelPath);

    std::string GetNameFromPath(std::string path);

public:
    AssetManager() = default;


    void LoadObject(class Object* obj);
    const std::unordered_map<std::string, std::shared_ptr<class Mesh>>& GetLoadedMeshes() { return LoadedMeshes; }
    const std::unordered_map<std::string, std::shared_ptr<class Texture>>& GetLoadedTextures() { return LoadedTextures; }

};

#endif //ASSETLOADER_H
