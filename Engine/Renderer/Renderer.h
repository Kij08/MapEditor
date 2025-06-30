//
// Created by Kij on 25/06/18.
//

#ifndef RENDERER_H
#define RENDERER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <optional>
#include <fstream>
#include <array>
#include "../Primitives/Mesh.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <memory>

#define VMA_IMPLEMENTATION
#include <span>

#include "../../include/vk_mem_alloc.h"

#include "../Primitives/Object.h"

class Window;
//The base of the Renderer class was created with help of the Vulkan Tutorial: https://vulkan-tutorial.com/Introduction

class Renderer {


public:

	static Renderer& Get();


	const uint32_t WIDTH = 1280;
	const uint32_t HEIGHT = 720;

	void Terminate();

	void Startup();
	GLFWwindow* GetWindow() { return windowRef; }

	void DrawFrame(const std::vector<std::shared_ptr<Object>>& objects);

	VkDevice GetDevice() { return device; };

	void SetFrameBufferResized(bool resized) { framebufferResized = resized; };


	struct VmaBuffer {
		VkBuffer buffer;
		VmaAllocation allocation;
		VmaAllocationInfo info;
	};

	//Loading Models
	MeshBuffers UploadModel(std::span<Vertex> vertices, std::span<uint32_t> indices);
	//IMplement later: Renderer should have a list of weak pointers to mesh data so it doesn't have to keep loading the asteroid data
	void LoadTexture(std::string texturePath, int& objTextureIndex);

private:
	const int MAX_FRAMES_IN_FLIGHT = 2;
	uint32_t currentFrame = 0;
	bool framebufferResized = false;

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	VkDebugUtilsMessengerEXT DebugMessenger;

	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();
	void SetupDebugMessenger();
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	const std::vector<const char*> DeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		"VK_KHR_dynamic_rendering"
	};

	GLFWwindow* windowRef;

	//Create Vulkan instance
	void CreateVKInstance();
	VkInstance instance;

	//Choose physical hardware to run on
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	void ChooseDevice();
	bool GPUSuitable(VkPhysicalDevice gpu);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;


		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	//Map hardware to logical device and create queues
	VkDevice device;
	void CreateLogicalDevice();
	VkQueue graphicsQueue;

	//VMA allocator
	VmaAllocator vmaAllocator;
	void CreateVulkanAllocator();

	//Vulkan-GLFW window surface
	VkSurfaceKHR surface;
	void CreateSurface();

	//Create presentation queue
	VkQueue presentQueue;

	//Swap Chain creation
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;


	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void CreateSwapChain();
	void RecreateSwapChain();
	void CleanupSwapChain();

	//Image Views
	std::vector<VkImageView> swapChainImageViews;
	void CreateImageViews();

	//Graphics pipeline
	VkPipelineLayout PipelineLayout;
	VkPipeline GraphicsPipeline;
	VkPipelineLayout UnlitPipelineLayout;
	VkPipeline UnlitPipeline;

	void CreateGraphicsPipeline();
	void CreateUnlitPipeline();

	VkShaderModule CreateShaderModule(const std::vector<char>& code);

	//Command Pool
	VkCommandPool CommandPool;
	void CreateCommandPool();

	//Command Buffer
	std::vector<VkCommandBuffer> CommandBuffers;
	void CreateCommandBuffers();
	void RecordCommandBuffer(VkCommandBuffer CmdBuffer, uint32_t imageIndex, const std::vector<std::shared_ptr<Object>>& objects);

	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer CmdBuffer);

	//Drawing synchronisation
	std::vector<VkSemaphore> ImageAvailableSemaphores;
	std::vector<VkSemaphore> RenderFinishedSemaphores;
	std::vector<VkFence> InFlightFences;
	void CreateSyncObjects();

	//Vertex Processing
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	//Buffers
	VkBuffer VertexBuffer;
	VkDeviceMemory VertexBufferMemory;
	VkBuffer IndexBuffer;
	VkDeviceMemory IndexBufferMemory;

	//Buffer for objs loaded and objs spawned during play
	VmaBuffer LoadedObjVertexBuffer;
	VmaBuffer LoadedObjIndexBuffer;
	VmaBuffer TransientObjVertexBuffer;
	VmaBuffer TransientObjIndexBuffer;

	std::vector<VkBuffer> UniformBuffers; //Objects share the same UBOs since it just has proj and view info in it.
	std::vector<VkDeviceMemory> UniformBuffersMemory;
	std::vector<void*> UniformBuffersMapped;
	VkDeviceSize vertexBufferSize = 100000000; //allocate 100 megabytes for fun
	VkDeviceSize indexBuferSize = 100000000; //allocate 100 megabytes for fun
	int currentVertexBufferOffset = 0;
	int currentIndexBufferOffset = 0;

public:
	//For GPU resource: VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT flag
	//For staging buffer: Use VK_BUFFER_USAGE_TRANSFER_SRC_BIT usage. VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT flag.
	//For readback from compute: same usage. VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT flag
	//For uniform buffer VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT flag
	VmaBuffer CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags flags);

	void DestroyBuffer(const VmaBuffer& buffer);
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void CreateVertexBuffer();
	void UpdateVertexBuffer(int newObjVertexOffset, int newObjVertexSize);
	void CreateIndexBuffer();
	void UpdateIndexBuffer(int newObjIndexOffset, int newObjIndexSize);
	void CreateUniformBuffer();

private:
	//Descriptor Set
	VkDescriptorSetLayout DescriptorSetLayout;
	VkDescriptorPool DescriptorPool;
	std::vector<std::vector<VkDescriptorSet>> DescriptorSets; //Vector of vectors for obj descriptor sets. Each frame in flight has a sometimes differing set of the obj sets

	void CreateDescriptorSetLayout();
	void CreateDescriptorPool();
	void CreateDescriptorSets();

public:
	void UpdateDescriptorSets(int objTextureIndex);
private:
	//UBO
	struct UniformBufferObject {
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

	struct MeshPushConstant {
		glm::mat4 modelMatrix;

		float Ka;
		float Kd;
		float Ks;
	};

	void UpdateUniformBuffer();

	//Texture Images TODO: All this image stuff can go in the texture class
	std::vector<VkImage> TextureImages;
	std::vector<VkDeviceMemory> TextureImageMemories;
	std::vector<VkImageView> TextureImageViews;
	VkSampler TextureSampler;

public:
	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

	Texture CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);
	void CreateTextureImageView(int objTexIndex);

	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	void CreateTextureSampler();
private:
	struct TextureCacheData {
		std::string path;
		int indexInTexArray;
	};

	std::vector<TextureCacheData> TextureCache;

	//Depth buffer
	/*VkImage DepthImage;
	VkDeviceMemory DepthImageMemory;
	VkImageView DepthImageView;*/

	Texture DepthTexture;

	void CreateDepthResources();

	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat FindDepthFormat();
	bool HasStencilComponent(VkFormat format);

	struct ModelCacheData {
		std::string path;
		int vertexOffset;
		int indexOffset;
		int vertexSize;
		int indexSize;
	};

	std::vector<ModelCacheData> ModelCache;
};

#endif //RENDERER_H
