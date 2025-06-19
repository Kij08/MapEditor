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
	Mesh mesh;

	Transform objTransform;

	std::string PATH_TO_MODEL = "";
	std::string PATH_TO_TEXTURE = "";

	//TODO: Move this stuff to model/texture classes
	int modelVertexStartOffset;
	int modelVertexSizeInBuffer;
	int modelIndexStartOffset;
	int modelIndexSizeInBuffer;

	//Index for descriptor set AND for texture image arrays
	int descriptorSetIndex;

	//TODO: move to Material
	float Ka = 0;
	float Kd = 0;
	float Ks = 0;

public:
	const Transform GetTransform() { return objTransform; };
	void SetTransform(Transform t) { objTransform = t; };

	std::string GetModelPath() { return PATH_TO_MODEL; };
	std::string GetTexturePath() { return PATH_TO_TEXTURE; };

	void SetLevelReference(Scene* scene) { SceneRef = scene; }
	Scene* GetLevel() { return SceneRef; };

	void SetVertOffset(int offset) { modelVertexStartOffset = offset; };
	void SetVertSizeInBuffer(int size) { modelVertexSizeInBuffer = size; };
	void SetIndexOffset(int offset) { modelIndexStartOffset = offset; };
	void SetIndexSizeInOffset(int size) { modelIndexSizeInBuffer = size; };
	void SetDescriptorSetIndex(int index) { descriptorSetIndex = index; };

	const int GetVertOffset() { return modelVertexStartOffset; };
	const int GetVertSizeInBuffer() { return modelVertexSizeInBuffer; };
	const int GetIndexOffset() { return modelIndexStartOffset; };
	const int GetIndexSizeInBuffer() { return modelIndexSizeInBuffer; };
	const int GetDescriptorIndex() { return descriptorSetIndex; };

	const Mesh GetMesh() { return mesh; }
	void AddVertToObjMesh(Mesh::MeshData vert) { mesh.vertices.push_back(vert); };
	void AddIndexToObjMesh(uint32_t index) { mesh.indices.push_back(index); };

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
