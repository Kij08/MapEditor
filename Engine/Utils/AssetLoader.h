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
    std::unordered_map<std::string, std::shared_ptr<class Texture>> LoadedTextures;
    std::unordered_map<std::string, std::shared_ptr<Mesh>> LoadedMeshes;

    std::shared_ptr<Texture> LoadTexture(std::string texturePath, int& objTextureIndex);
    std::shared_ptr<Mesh> LoadMesh(std::string modelPath, int& vertexOffset, int& indexOffset, int& objVertexSize, int& objIndexSize);

public:
    AssetManager();


    Object* LoadObject(Object* obj);


};

#endif //ASSETLOADER_H
