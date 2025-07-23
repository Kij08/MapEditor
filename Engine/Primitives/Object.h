//
// Created by Kij on 25/06/18.
//

#ifndef OBJECT_H
#define OBJECT_H

#include "string"
#include <memory>
#include "Empty.h"
#include "Components/MeshComponent.h"

class Scene;
class MeshComponent;

//Base class for anything that is rendered in the scene
class Object : public Empty {

protected:
	//std::shared_ptr<Mesh> MeshRef;
	//std::shared_ptr<Texture> TextureRef;
	//std::shared_ptr<Material> MaterialRef;

	std::unique_ptr<MeshComponent> meshComponent;

	//TODO: move to Material
	float Ka = 0;
	float Kd = 0;
	float Ks = 0;

public:

	MeshComponent* GetMeshComponent() { return meshComponent.get(); };


	float GetKa() const { return Ka; }
	float GetKd() { return Kd; }
	float GetKs() { return Ks; }

	explicit Object(Scene* s);
	Object(Scene* s, std::string name);
	Object(Scene* s, std::string name, Transform t);
	Object(Scene* s, std::string model, std::string tex);
	Object(Scene* s, Transform t, const MeshComponentDefinition& mc);

	bool bIsDirty;

	void Tick() override;
	void Begin() override;
	ObjectDefinition GetObjectDefinition() override;
};


#endif //OBJECT_H
