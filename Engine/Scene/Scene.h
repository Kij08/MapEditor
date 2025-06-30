//
// Created by Kij on 25/06/18.
//

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "../Utils/AssetLoader.h"

struct ObjectMaterialGrouping {

};

class Scene {

protected:
    //List of all objects, grouped by material
    std::vector<ObjectMaterialGrouping> materialGroupings;
    std::vector<std::shared_ptr<Object>> objects;

    AssetManager sceneAssetManager;
private:

public:

    void InitScene();

    template<class TObject>
    TObject* CreateObject();
};

#endif //SCENE_H
