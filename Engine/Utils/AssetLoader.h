//
// Created by Kij on 25/06/19.
//

#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

enum class ETextureType {
    WorldTexture,
    UITexture
};

class AssetManager {
    std::unordered_map<std::string, std::shared_ptr<class Texture>> LoadedTextures;
    std::unordered_map<std::string, std::shared_ptr<class Mesh>> LoadedMeshes;

    std::string GetNameFromPath(std::string path);

public:
    AssetManager() = default;

    //Load individual types
    std::shared_ptr<Texture> LoadTexture(std::string texturePath, ETextureType textureType);
    std::shared_ptr<Mesh> LoadMesh(std::string modelPath);

    //Load Components
    void LoadMeshComponent(class MeshComponent* meshComp);

    Texture* GetTexture(std::string texturePath) { return LoadedTextures.find(texturePath)->second.get(); }


    const std::unordered_map<std::string, std::shared_ptr<class Mesh>>& GetLoadedMeshes() { return LoadedMeshes; }
    const std::unordered_map<std::string, std::shared_ptr<class Texture>>& GetLoadedTextures() { return LoadedTextures; }

};

#endif //ASSETLOADER_H
