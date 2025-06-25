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
class Object;
class Window;
//The base of the Renderer class was created with help of the Vulkan Tutorial: https://vulkan-tutorial.com/Introduction

class Renderer {

public:
	const uint32_t WIDTH = 1280;
	const uint32_t HEIGHT = 720;

	void Terminate();

	void Startup();
	GLFWwindow* GetWindow() { return windowRef; }

	void DrawFrame(const std::vector<std::shared_ptr<Object>>& objects);

	VkDevice GetDevice() { return device; };

	void SetFrameBufferResized(bool resized) { framebufferResized = resized; };

	struct Vertex { //This struct holds mesh vertex data and decribes the bindings for vertex shader data
		Mesh::MeshData mesh;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, mesh.pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, mesh.colour);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, mesh.uv);

			attributeDescriptions[3].binding = 0;
			attributeDescriptions[3].location = 3;
			attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[3].offset = offsetof(Vertex, mesh.normal);

			return attributeDescriptions;
		}

		bool operator==(const Vertex& other) const {
			return mesh.pos == other.mesh.pos && mesh.colour == other.mesh.colour && mesh.uv == other.mesh.uv && mesh.normal == other.mesh.normal;
		}
	};

	//Loading Models
	void LoadModel(int& vertexOffset, int& indexOffset, int& objVertexSize, int& objIndexSize, std::string modelPath);
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

	//Render Pass
	VkRenderPass RenderPass;
	void CreateRenderPass();

	//Frame buffers
	std::vector<VkFramebuffer> swapChainFramebuffers;
	void CreateFrameBuffers();

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
	std::vector<VkBuffer> UniformBuffers; //Objects share the same UBOs since it just has proj and view info in it.
	std::vector<VkDeviceMemory> UniformBuffersMemory;
	std::vector<void*> UniformBuffersMapped;
	VkDeviceSize vertexBufferSize = 100000000; //allocate 100 megabytes for fun
	VkDeviceSize indexBuferSize = 100000000; //allocate 100 megabytes for fun
	int currentVertexBufferOffset = 0;
	int currentIndexBufferOffset = 0;

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void CreateVertexBuffer();
	void UpdateVertexBuffer(int newObjVertexOffset, int newObjVertexSize);
	void CreateIndexBuffer();
	void UpdateIndexBuffer(int newObjIndexOffset, int newObjIndexSize);
	void CreateUniformBuffer();

	//Descriptor Set
	VkDescriptorSetLayout DescriptorSetLayout;
	VkDescriptorPool DescriptorPool;
	std::vector<std::vector<VkDescriptorSet>> DescriptorSets; //Vector of vectors for obj descriptor sets. Each frame in flight has a sometimes differing set of the obj sets

	void CreateDescriptorSetLayout();
	void CreateDescriptorPool();
	void CreateDescriptorSets();
	void UpdateDescriptorSets(int objTextureIndex);

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

	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void CreateTextureImageView(int objTexIndex);

	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	void CreateTextureSampler();

	struct TextureCacheData {
		std::string path;
		int indexInTexArray;
	};

	std::vector<TextureCacheData> TextureCache;

	//Depth buffer
	VkImage DepthImage;
	VkDeviceMemory DepthImageMemory;
	VkImageView DepthImageView;

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
