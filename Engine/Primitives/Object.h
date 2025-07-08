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
#include "Empty.h"

class Scene;


//Base class for anything that is rendered in the scene
class Object : public Empty {

protected:
	std::shared_ptr<Mesh> MeshRef;
	std::shared_ptr<Texture> TextureRef;
	std::shared_ptr<Material> MaterialRef;

	std::string PATH_TO_MODEL = "";
	std::string PATH_TO_TEXTURE = "";

	//TODO: move to Material
	float Ka = 0;
	float Kd = 0;
	float Ks = 0;

public:
	void SetMesh(std::shared_ptr<Mesh> m) { MeshRef = m; };
	void SetTexture(std::shared_ptr<Texture> m) { TextureRef = m; };

	Mesh* GetMesh() { return MeshRef.get(); };
	Texture* GetTexture() { return TextureRef.get(); };

	std::string GetModelPath() { return PATH_TO_MODEL; };
	std::string GetTexturePath() { return PATH_TO_TEXTURE; };



	float GetKa() const { return Ka; }
	float GetKd() { return Kd; }
	float GetKs() { return Ks; }

	Object();
	Object(std::string model, std::string tex);

	bool bIsDirty;

	void Tick() override;
	void Begin() override;
};


#endif //OBJECT_H
