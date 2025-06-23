//
// Created by Kij on 25/06/18.
//

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "../Primitives/Object.h"
#include <unordered_map>

struct ObjectMaterialGrouping {

};

class Scene {

protected:
    //List of all objects, grouped by material
    std::vector<ObjectMaterialGrouping> materialGroupings;

    //Assets loaded for this scene
    std::unordered_map<std::string, std::shared_ptr<Mesh>> LoadedMeshes;
    std::unordered_map<std::string, std::shared_ptr<Texture>> LoadedTextures;

private:

public:

    void InitScene();
    void AddObject(Object* obj);
};



#endif //SCENE_H
