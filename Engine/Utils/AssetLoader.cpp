//
// Created by Kij on 25/06/19.
//

#include "AssetLoader.h"
#include "../Primitives/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../include/stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../include/tiny_obj_loader.h"

#include <iostream>

std::shared_ptr<Texture> AssetManager::LoadTexture(std::string texturePath)
{
	//If the texture is already loaded in the map then return it here
	auto loadedTex = LoadedTextures.find(texturePath);
	if (loadedTex != LoadedTextures.end()) {
		//Return the pointer of the texture
		return loadedTex->second;
	}


	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels) {
		throw std::runtime_error("Could not load texture image!");
	}

	Renderer::VmaBuffer stagingBuffer = Renderer::Get().CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

	//Copy pixel mem to buffer then free the raw data
	memcpy(stagingBuffer.info.pMappedData, pixels, imageSize);
	stbi_image_free(pixels);

	Texture tex = Renderer::Get().CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

	Renderer::Get().TransitionImageLayout(tex.GetAllocation().TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	Renderer::Get().CopyBufferToImage(stagingBuffer.buffer, tex.GetAllocation().TextureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	Renderer::Get().TransitionImageLayout(tex.GetAllocation().TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	Renderer::Get().DestroyBuffer(stagingBuffer);

	//Update descriptor sets

	return std::make_shared<Texture>(tex);

}

std::shared_ptr<Mesh> AssetManager::LoadMesh(std::string modelPath)
{
	//Check if mesh is already loaded
	auto loadedMesh = LoadedMeshes.find(modelPath);
	if (loadedMesh != LoadedMeshes.end()) {
		//Return the pointer of the texture
		return loadedMesh->second;
	}

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err,modelPath.c_str())) {
		std::cout << "Could not load model";
		throw std::runtime_error(err);
	}

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			//Array of floats so we need an offset of 3
			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			//Array of floats so we need an offset of 2. Flip UV Y for vulkan
			vertex.uv = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.normal = {
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};

			vertex.colour = { 1.0f, 1.0f, 1.0f };

			//If vertex isnt in uniquevertices set its index to its index in verticies and add it. If its already in just add its index to indicies.
			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());

				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}

	//Upload mesh to GPU
	Mesh mesh(Renderer::Get().UploadModel(vertices, indices));
	return std::make_shared<Mesh>(mesh);
}

void AssetManager::LoadObject(Object *obj) {
	std::shared_ptr<Mesh> mesh = LoadMesh(obj->GetModelPath());
	std::shared_ptr<Texture> texture = LoadTexture(obj->GetTexturePath());

	obj->SetMesh(mesh);
	obj->SetTexture(texture);

	LoadedMeshes.emplace(obj->GetModelPath(), mesh);
	LoadedTextures.emplace(obj->GetTexturePath(), texture);
}
