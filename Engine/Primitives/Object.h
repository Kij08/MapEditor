//
// Created by Kij on 25/06/18.
//

#ifndef OBJECT_H
#define OBJECT_H



#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "string"
#include <memory>

class Scene;

struct Transform {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

class Object {

public:

protected:
	std::shared_ptr<Mesh> Mesh;
	std::shared_ptr<Texture> Texture;
	std::shared_ptr<Material> Material;
	Transform objTransform;

	std::string PATH_TO_MODEL = "";
	std::string PATH_TO_TEXTURE = "";

	//TODO: move to Material
	float Ka = 0;
	float Kd = 0;
	float Ks = 0;

public:
	void SetMesh(std::shared_ptr<class Mesh> m) { Mesh = m; };
	void SetTexture(std::shared_ptr<class Texture> m) { Texture = m; };

	const Transform GetTransform() { return objTransform; };
	void SetTransform(Transform t) { objTransform = t; };

	std::string GetModelPath() { return PATH_TO_MODEL; };
	std::string GetTexturePath() { return PATH_TO_TEXTURE; };

	void SetLevelReference(Scene* scene) { SceneRef = scene; }
	Scene* GetLevel() { return SceneRef; };

	float GetKa() const { return Ka; }
	float GetKd() { return Kd; }
	float GetKs() { return Ks; }

	Object();
	Object(Transform t, std::string model, std::string tex);

	virtual void Tick();

	bool bIsDirty;

private:
	Scene* SceneRef;
};


#endif //OBJECT_H
