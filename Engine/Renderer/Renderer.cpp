//
// Created by Kij on 25/06/18.
//

#include "Renderer.h"
#include <stdexcept>
#include <set>
#include <limits>
#include <algorithm>
#include <iostream>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cstring>
#include <glm/gtc/matrix_transform.hpp>

#define VMA_IMPLEMENTATION
#include "../../include/vk_mem_alloc.h"

static Renderer* loadedRenderer;

Renderer& Renderer::Get() {
	if (loadedRenderer == nullptr) {
		loadedRenderer = new Renderer();
	}
	return *loadedRenderer;
}

//Static functions
static std::vector<char> ReadShaderFiles(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Could not open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

static void ResizeFrameBufferCallback(GLFWwindow* window, int width, int height) {
	Renderer* ren = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
	if (ren) {
		ren->SetFrameBufferResized(true);
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}
//--------GLFW Functions

void Renderer::Terminate()
{
	CleanupSwapChain();

	vkDestroySampler(device, TextureSampler, nullptr);
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

	}

	vkDestroyDescriptorPool(device, DescriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(device, DescriptorSetLayout, nullptr);

	vkDestroyCommandPool(device, CommandPool, nullptr);

	vmaDestroyAllocator(vmaAllocator);

	vkDestroyPipeline(device, UnlitPipeline, nullptr);
	vkDestroyPipelineLayout(device, UnlitPipelineLayout, nullptr);
	vkDestroyPipeline(device, GraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device, PipelineLayout, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(device, RenderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, ImageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, InFlightFences[i], nullptr);
	}

	vkDestroyDevice(device, nullptr);

	vkDestroySurfaceKHR(instance, surface, nullptr);

	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, DebugMessenger, nullptr);
	}

	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(GetWindow());

	glfwTerminate();
}

void Renderer::Startup() {

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	windowRef = glfwCreateWindow(WIDTH, HEIGHT, "Map Editor", nullptr, nullptr);
	glfwSetWindowUserPointer(windowRef, this);
	glfwSetFramebufferSizeCallback(windowRef, ResizeFrameBufferCallback);

	CreateVKInstance();
	SetupDebugMessenger();
	CreateSurface();
	ChooseDevice();
	CreateLogicalDevice();
	CreateVulkanAllocator();
	CreateSwapChain();
	CreateImageViews();
	CreateDescriptorSetLayout();
	InitDescriptors();
	CreateGraphicsPipeline();
	//CreateUnlitPipeline();
	CreateDepthResources();
	CreateCommandPool();
	CreateTextureSampler();
	CreateUniformBuffer();
	CreateDescriptorPool();
	CreateDescriptorSets();
	CreateCommandBuffers();
	CreateSyncObjects();
}


//Draw call
void Renderer::DrawFrame(const std::vector<std::shared_ptr<Object>>& objects)
{
	//Wait for previous frame to finish
	vkWaitForFences(device, 1, &InFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, ImageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Could not acquire swap chain image!");
	}

	vkResetFences(device, 1, &InFlightFences[currentFrame]);

	vkResetCommandBuffer(CommandBuffers[currentFrame], 0);

	RecordCommandBuffer(CommandBuffers[currentFrame], imageIndex, currentFrame, objects);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { ImageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &CommandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = { RenderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, InFlightFences[currentFrame]) != VK_SUCCESS) {
		std::cout << "could submit cquuee";
		throw std::runtime_error("Could not submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		RecreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		std::cout << "Swapchain stuff";
		throw std::runtime_error("Coud not present swap chain image!");
	}


	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

//Vk Setup

bool Renderer::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (std::strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

//Get required vulkan Instance extensions
std::vector<const char*> Renderer::GetRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

	return extensions;
}


void Renderer::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

VkResult Renderer::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void Renderer::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

void Renderer::SetupDebugMessenger()
{
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	PopulateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &DebugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

void Renderer::CreateVKInstance()
{

	if (enableValidationLayers && !CheckValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Map Editor";
	appInfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("Could not create instance");
	}
}

void Renderer::ChooseDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("No GPU with Vulkan support found");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const VkPhysicalDevice device : devices) {
		if (GPUSuitable(device)) {
			physicalDevice = device;
			break;
		}
		else {
			std::cout << "Could not find a suitable GPU!" << std::endl;
		}
	}
}

bool Renderer::GPUSuitable(VkPhysicalDevice gpu)
{
	VkPhysicalDeviceBufferDeviceAddressFeatures addressFeatures { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES };
	VkPhysicalDeviceProperties2 deviceProperties = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
	VkPhysicalDeviceFeatures2 deviceFeatures = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &addressFeatures};
	vkGetPhysicalDeviceProperties2(gpu, &deviceProperties);
	vkGetPhysicalDeviceFeatures2(gpu, &deviceFeatures);

	if (deviceFeatures.features.samplerAnisotropy != true) {
		return false;
	}
	if (addressFeatures.bufferDeviceAddress != true) { //
		return false;
	}


	QueueFamilyIndices indices = findQueueFamilies(gpu);

	//Prefer discrete gpu over integrated
	std::cout << "Device Support: " << CheckDeviceExtensionSupport(gpu) << std::endl;
	if (indices.isComplete() && CheckDeviceExtensionSupport(gpu)) {
		bool swapChainAdequate = false;
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(gpu);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

		if (deviceProperties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			return swapChainAdequate; //will be true if swapchain is supported
		}
		else if (deviceProperties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
			return swapChainAdequate;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

bool Renderer::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

Renderer::QueueFamilyIndices Renderer::findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		i++;
	}

	return indices;
}

void Renderer::CreateLogicalDevice() {
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	VkDeviceQueueCreateInfo queueCreateInfo{};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	//Enable Vulkan core features
	VkPhysicalDeviceSynchronization2Features sync2Features = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
		.synchronization2 = VK_TRUE
	};

	VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
		.pNext = &sync2Features,
		.dynamicRendering = VK_TRUE
	};

	VkPhysicalDeviceBufferDeviceAddressFeatures bufferFeatures {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES,
		.pNext = &dynamicRenderingFeatures,
		.bufferDeviceAddress = VK_TRUE
	};

	VkPhysicalDeviceFeatures2 deviceFeatures {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
		.pNext = &bufferFeatures,
		.features{ .samplerAnisotropy = VK_TRUE }
	};

	VkDeviceCreateInfo createInfo{  };
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = DeviceExtensions.data();
	createInfo.pNext = &deviceFeatures;


	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("Could not create logical device");
	}

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void Renderer::CreateVulkanAllocator() {
	VmaVulkanFunctions vulkanFunctions = {};
	vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
	vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo allocatorCreateInfo = {};
	allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT | VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_4;
	allocatorCreateInfo.physicalDevice = physicalDevice;
	allocatorCreateInfo.device = device;
	allocatorCreateInfo.instance = instance;
	allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

	vmaCreateAllocator(&allocatorCreateInfo, &vmaAllocator);
}

void Renderer::CreateSurface()
{
	if (glfwCreateWindowSurface(instance, GetWindow(), nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("Could not create window surface");
	}
}

Renderer::SwapChainSupportDetails Renderer::QuerySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR Renderer::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}

VkPresentModeKHR Renderer::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			//return availablePresentMode; //Uncomment to uncap FPS
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR; // This will force VSync
}

VkExtent2D Renderer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(GetWindow(), &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void Renderer::CreateSwapChain()
{
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("Could not create swap chain!");
	}

	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void Renderer::RecreateSwapChain()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(GetWindow(), &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(GetWindow(), &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(device);

	CleanupSwapChain();

	CreateSwapChain();
	CreateImageViews();
	CreateDepthResources();
}

void Renderer::CleanupSwapChain()
{
	vkDestroyImageView(device, DepthTexture.TextureImageView, nullptr);
	vmaDestroyImage(vmaAllocator, DepthTexture.TextureImage, DepthTexture.TextureMem);

	for (auto imageView : swapChainImageViews) {
		vkDestroyImageView(device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(device, swapChain, nullptr);
}

//Create image views for swapchain
void Renderer::CreateImageViews()
{
	swapChainImageViews.resize(swapChainImages.size());

	for (uint32_t i = 0; i < swapChainImages.size(); i++) {
		swapChainImageViews[i] = CreateImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

void Renderer::CreateGraphicsPipeline()
{
	std::vector<char> vertShaderCode = ReadShaderFiles("../Engine/Shaders/vert.spv");
	std::vector<char> fragShaderCode = ReadShaderFiles("../Engine/Shaders/frag.spv");

	VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	//VkVertexInputBindingDescription bindingDescription = Vertex::getBindingDescription();
	//auto attributeDescriptions = Vertex::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	/*vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();*/

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;

	std::vector<VkDynamicState> dynamicStates = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional



	//setup push constants
	VkPushConstantRange push_constant;
	//this push constant range starts at the beginning
	push_constant.offset = 0;
	//this push constant range takes up the size of a MeshPushConstants struct
	push_constant.size = sizeof(MeshPushConstant);
	//this push constant range is accessible only in the vertex shader
	push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &DescriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &push_constant;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Could not create pipeline layout!");
	}

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	const VkPipelineRenderingCreateInfoKHR PipelineRenderingCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &swapChainImageFormat,
		.depthAttachmentFormat = FindDepthFormat(),
	};

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = &PipelineRenderingCreateInfo,
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = PipelineLayout;
	pipelineInfo.renderPass = VK_NULL_HANDLE;
	pipelineInfo.subpass = 0;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &GraphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("Could not create graphics pipeline!");
	}

	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

void Renderer::CreateUnlitPipeline()
{
	std::vector<char> vertShaderCode = ReadShaderFiles("Source/Shaders/skybox-vert.spv");
	std::vector<char> fragShaderCode = ReadShaderFiles("Source/Shaders/skybox-frag.spv");

	VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	//setup push constants
	VkPushConstantRange push_constant;
	//this push constant range starts at the beginning
	push_constant.offset = 0;
	//this push constant range takes up the size of a MeshPushConstants struct
	push_constant.size = sizeof(MeshPushConstant);
	//this push constant range is accessible only in the vertex shader
	push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &DescriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &push_constant;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &UnlitPipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Could not create unlit pipeline layout!");
	}

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = UnlitPipelineLayout;
	pipelineInfo.renderPass = VK_NULL_HANDLE;
	pipelineInfo.subpass = 0;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &UnlitPipeline) != VK_SUCCESS) {
		throw std::runtime_error("Could not create unlit graphics pipeline!");
	}

	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

VkShaderModule Renderer::CreateShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("Could not create shader module!");
	}

	return shaderModule;
}

void Renderer::CreateCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &CommandPool) != VK_SUCCESS) {
		throw std::runtime_error("Could not create command pool!");
	}
}

void Renderer::CreateCommandBuffers()
{
	CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)CommandBuffers.size();

	if (vkAllocateCommandBuffers(device, &allocInfo, CommandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void Renderer::RecordCommandBuffer(VkCommandBuffer CmdBuffer, uint32_t imageIndex, int frameIndex, const std::vector<std::shared_ptr<Object>>& objects)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(CmdBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Could not begin recording command buffer!");
	}

	//Image transitions from undefined to attach for optimal attachment before rendering
	const VkImageMemoryBarrier ImgMemBarrierColourAttach {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.image = swapChainImages[imageIndex],
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		  }
	};

	vkCmdPipelineBarrier(
		CmdBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,  // srcStageMask
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
		0,
		0,
		nullptr,
		0,
		nullptr,
		1, // imageMemoryBarrierCount
		&ImgMemBarrierColourAttach // pImageMemoryBarriers
	);

	const VkImageMemoryBarrier ImgMemBarrierDepthAttach {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
		.image = DepthTexture.TextureImage,
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		  }
	};

	vkCmdPipelineBarrier(
	CmdBuffer,
	VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,  // srcStageMask
	VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, // dstStageMask
	0,
	0,
	nullptr,
	0,
	nullptr,
	1, // imageMemoryBarrierCount
	&ImgMemBarrierDepthAttach // pImageMemoryBarriers
	);


	//Info for dynamic rendering
	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	const VkRenderingAttachmentInfoKHR ColourAttachInfo {
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
		.imageView = swapChainImageViews[imageIndex],
		.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.clearValue = clearValues[0]
	};

	const VkRenderingAttachmentInfoKHR DepthAttachInfo {
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
		.imageView = DepthTexture.TextureImageView,
		.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
		.resolveMode = VK_RESOLVE_MODE_NONE,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.clearValue = clearValues[1]
	};

	VkRect2D renderingArea = {VkOffset2D{}, VkExtent2D{WIDTH, HEIGHT}};

	const VkRenderingInfoKHR RenderInfo {
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
		.renderArea = renderingArea,
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &ColourAttachInfo,
		.pDepthAttachment = &DepthAttachInfo
	};

	UpdateUniformBuffer(CmdBuffer, frameIndex); //Update uniform buffer for this frame

	///Start Draw
	vkCmdBeginRendering(CmdBuffer, &RenderInfo);

	//Since these are dynamic they must be set here
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapChainExtent.width);
	viewport.height = static_cast<float>(swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(CmdBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;
	vkCmdSetScissor(CmdBuffer, 0, 1, &scissor);


	//Bind lit pipeline
	vkCmdBindPipeline(CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline);

	//Make vertex buffer big and put all object verts in there, if it ever gets too small double its size using the same createVertexBuffer() workflow
	//Need to store the offsets for each object, be able to remove and add objects to vert and index array
	//UpdateUniformBuffer(currentFrame, obj->GetTransform().position, obj->GetTransform().rotation, obj->GetTransform().scale);
	frames[frameIndex].Descriptors.ClearDescriptors(device);

	for (auto const& obj : objects) {

		TextureAllocation objTex = obj.get()->GetTexture()->GetAllocation();
		//Dynamically allocate descriptors for obj textures. Use global set layout
		VkDescriptorSet imageSet = frames[frameIndex].Descriptors.AllocateDescriptorSet(device, DescriptorSetLayout);

		DescriptorWriter writer;
		writer.WriteBuffer(0, frames[frameIndex].UniformBuffer.buffer, sizeof(UniformBufferObject), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER); //TODO: Cache UBO. Don't need to update it every frame
		writer.WriteImage(1, objTex.TextureImageView, TextureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

		writer.UpdateSet(device, imageSet);

		vkCmdBindDescriptorSets(CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout, 0, 1, &imageSet, 0, nullptr);

		vkCmdBindIndexBuffer(CmdBuffer, obj.get()->GetMesh()->GetIndexBuffer().buffer, 0, VK_INDEX_TYPE_UINT32);

		static float xRot = 30;
		Transform t{ .position = glm::vec3(0, 0, 0), .rotation = glm::vec3(xRot, 0, 0), .scale = glm::vec3(0.5, 0.5, 0.5)};
		xRot += 0.1;
		obj->SetTransform(t);
		//The magic function
		glm::mat4 modelMatrix(1.f);
		modelMatrix = glm::translate(modelMatrix, obj->GetTransform().position);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(obj->GetTransform().rotation.x), glm::vec3(1.f, 0, 0));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(obj->GetTransform().rotation.y), glm::vec3(0, 1.f, 0));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(obj->GetTransform().rotation.z), glm::vec3(0, 0, 1.f));
		modelMatrix = glm::scale(modelMatrix, obj->GetTransform().scale);

		//Set up push constants. Model matrix and blinn-hong shading lighting constants
		MeshPushConstant constant;
		constant.modelMatrix = modelMatrix;
		constant.vBufAddress = obj->GetMesh()->GetVertexBufferAddress();
		constant.Ka = obj->GetKa();
		constant.Kd = obj->GetKd();
		constant.Ks = obj->GetKs();

		vkCmdPushConstants(CmdBuffer, PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstant), &constant);
		vkCmdDrawIndexed(CmdBuffer, obj.get()->GetMesh()->primitives[0].count, 1, 0, 0, 0);
	}

	vkCmdEndRendering(CmdBuffer);

	//Transition colour swapchain images to present bit for optimal rendering
	const VkImageMemoryBarrier ImgMemBarrierPresentTransition {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		.image = swapChainImages[imageIndex],
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		  }
	};

	vkCmdPipelineBarrier(
		CmdBuffer,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // srcStageMask
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, // dstStageMask
		0,
		0,
		nullptr,
		0,
		nullptr,
		1, // imageMemoryBarrierCount
		&ImgMemBarrierPresentTransition // pImageMemoryBarriers
	);



	if (vkEndCommandBuffer(CmdBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Could not record command buffer!");
	}
}

void Renderer::CreateSyncObjects()
{
	ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &ImageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &RenderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &InFlightFences[i]) != VK_SUCCESS) {

			throw std::runtime_error("Could not create synchronization objects for a frame!");
		}
	}
}

VmaBuffer Renderer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags flags)
{
	VkBufferCreateInfo bufferInfo = {.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
	bufferInfo.pNext = nullptr;
	bufferInfo.size = size;
	bufferInfo.usage = usage;

	VmaAllocationCreateInfo vmaAllocInfo = {};
	vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	vmaAllocInfo.flags = flags;
	VmaBuffer newBuffer;

	vmaCreateBuffer(vmaAllocator, &bufferInfo, &vmaAllocInfo, &newBuffer.buffer, &newBuffer.allocation, &newBuffer.info);
	return newBuffer;
}

void Renderer::DestroyBuffer(const VmaBuffer &buffer) {
	vmaDestroyBuffer(vmaAllocator, buffer.buffer, buffer.allocation);
}

void Renderer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	//Implement seperate command pool for short lived command buffers? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT

	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	EndSingleTimeCommands(commandBuffer);
}

VkCommandBuffer Renderer::BeginSingleTimeCommands() {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = CommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void Renderer::EndSingleTimeCommands(VkCommandBuffer CmdBuffer) {
	vkEndCommandBuffer(CmdBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &CmdBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(device, CommandPool, 1, &CmdBuffer);
}

uint32_t Renderer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("Could not find suitable memory type!");
}

void Renderer::CreateUniformBuffer()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		frames[i].UniformBuffer = CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
	}

}

void Renderer::InitDescriptors() {

	//Init initial descriptor pool for each frame
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		// create a descriptor pool
		std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> frame_sizes = {
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 },
		};

		frames[i].Descriptors = DescriptorAllocatorGrowable{};
		frames[i].Descriptors.InitPool(device, 1000, frame_sizes);
	}

}

void Renderer::CreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;


	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();


	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &DescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("Could not create descriptor set layout!");
	}
}

void Renderer::CreateDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 6 * 2);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 6 * 2);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 6 * 2);


	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &DescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("Could not create descriptor pool!");
	}
}

void Renderer::CreateDescriptorSets() //Each object probably needs a descriptor set. Each object needs its own UBO. Each frame in flight needs its own set of all of these
{ //You can do both: put the camera view and projection matrices in a uniform buffer and use a push constant for the per model matrices.


  //Try to make 4 descriptor sets: 1 for each texture, use push constants to change the model matrix


	std::vector<VkDescriptorSetLayout> layouts(static_cast<uint32_t>(6 * 2), DescriptorSetLayout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = DescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(6 * 2);
	allocInfo.pSetLayouts = layouts.data();

	DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT); //Resize frames in flight size to max frames in flight

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		DescriptorSets[i].resize(6 * 2); //resize obj spaces for max frame in flight * max number of objects I have in scene + 1 * double

		if (vkAllocateDescriptorSets(device, &allocInfo, DescriptorSets[i].data()) != VK_SUCCESS) {
			std::cout << "Couldnt allocate sets";
			throw std::runtime_error("Could not allocate descriptor sets!");
		}
	}
}

void Renderer::UpdateUniformBuffer(VkCommandBuffer CmdBuf, int frameIndex)
{
	UniformBufferObject ubo{};

	ubo.view = glm::lookAt(glm::vec3(500.0f, 30.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	ubo.proj = glm::perspective(glm::radians(70.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 1000.f);

	//Y axis transformation since glm is for OpenGL
	ubo.proj[1][1] *= -1;

	//Test for what kind of UBO Vma made. If its in BAR or RAM, or if its on the GPU.
	VkMemoryPropertyFlags memPropFlags;
	vmaGetAllocationMemoryProperties(vmaAllocator, frames[frameIndex].UniformBuffer.allocation, &memPropFlags);

	if(memPropFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
	    // The Allocation ended up in a mappable memory.
	    vmaCopyMemoryToAllocation(vmaAllocator, &ubo, frames[frameIndex].UniformBuffer.allocation, 0, sizeof(UniformBufferObject));

	    VkBufferMemoryBarrier bufMemBarrier = { VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
	    bufMemBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
	    bufMemBarrier.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT;
	    bufMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	    bufMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	    bufMemBarrier.buffer = frames[frameIndex].UniformBuffer.buffer;
	    bufMemBarrier.offset = 0;
	    bufMemBarrier.size = VK_WHOLE_SIZE;

	    // It's important to insert a buffer memory barrier here to ensure writing to the buffer has finished.
	    vkCmdPipelineBarrier(CmdBuf, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
	        0, 0, nullptr, 1, &bufMemBarrier, 0, nullptr);
	}
	else {
	    // Allocation ended up in a non-mappable memory - a transfer using a staging buffer is required.

		VmaBuffer stagingBuffer = CreateBuffer(sizeof(UniformBufferObject), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
			VMA_ALLOCATION_CREATE_MAPPED_BIT);

	    // Calling vmaCopyMemoryToAllocation() does vmaMapMemory(), memcpy(), vmaUnmapMemory(), and vmaFlushAllocation().
	    vmaCopyMemoryToAllocation(vmaAllocator, &ubo, stagingBuffer.allocation, 0, sizeof(UniformBufferObject));


	    VkBufferMemoryBarrier bufMemBarrier = { VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
	    bufMemBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
	    bufMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	    bufMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	    bufMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	    bufMemBarrier.buffer = stagingBuffer.buffer;
	    bufMemBarrier.offset = 0;
	    bufMemBarrier.size = VK_WHOLE_SIZE;

	    // Insert a buffer memory barrier to make sure writing to the staging buffer has finished.
	    vkCmdPipelineBarrier(CmdBuf, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
	        0, 0, nullptr, 1, &bufMemBarrier, 0, nullptr);

	    VkBufferCopy bufCopy = {
	        0, // srcOffset
	        0, // dstOffset,
	        sizeof(UniformBufferObject), // size
	    };

	    vkCmdCopyBuffer(CmdBuf, stagingBuffer.buffer, frames[frameIndex].UniformBuffer.buffer, 1, &bufCopy);

	    VkBufferMemoryBarrier bufMemBarrier2 = { VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
	    bufMemBarrier2.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	    bufMemBarrier2.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT; // We created a uniform buffer
	    bufMemBarrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	    bufMemBarrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	    bufMemBarrier2.buffer = frames[frameIndex].UniformBuffer.buffer;
	    bufMemBarrier2.offset = 0;
	    bufMemBarrier2.size = VK_WHOLE_SIZE;

	    // Make sure copying from staging buffer to the actual buffer has finished by inserting a buffer memory barrier.
	    vkCmdPipelineBarrier(CmdBuf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
	        0, 0, nullptr, 1, &bufMemBarrier2, 0, nullptr);
	}

}

Texture Renderer::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = static_cast<uint32_t>(width);
	imageInfo.extent.height = static_cast<uint32_t>(height);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0; // Optional

	VmaAllocationCreateInfo vmaAllocationCreateInfo = {};
	vmaAllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
	vmaAllocationCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	vmaAllocationCreateInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	TextureAllocation TexAlloc;

	vmaCreateImage(vmaAllocator, &imageInfo, &vmaAllocationCreateInfo, &TexAlloc.TextureImage, &TexAlloc.TextureMem, nullptr);

	//If we are making a depth image use the proper aspect flag
	VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
	if (format == VK_FORMAT_D32_SFLOAT) {
		aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
	}

	TexAlloc.TextureImageView = CreateImageView(TexAlloc.TextureImage, format, aspectFlag);

	Texture Tex(TexAlloc);
	return Tex;
}

VkImageView Renderer::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("Could not create texture image view!");
	}

	return imageView;
}

void Renderer::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	//Test for different layout configs and set the correct barriers
	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage, 0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	EndSingleTimeCommands(commandBuffer);
}

void Renderer::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	EndSingleTimeCommands(commandBuffer);
}

void Renderer::CreateTextureSampler()
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;

	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);

	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(device, &samplerInfo, nullptr, &TextureSampler) != VK_SUCCESS) {
		throw std::runtime_error("Could not create texture sampler!");
	}
}

void Renderer::CreateDepthResources()
{
	VkFormat depthFormat = FindDepthFormat();
	DepthTexture = CreateImage(swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT).GetAllocation();
}

VkFormat Renderer::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("Could not find supported format!");
}

VkFormat Renderer::FindDepthFormat() {
	return FindSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

bool Renderer::HasStencilComponent(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

MeshBuffers Renderer::UploadModel(std::span<Vertex> vertices, std::span<uint32_t> indices) {
	const size_t vertexBufferSize = vertices.size() * sizeof(Vertex);
	const size_t indexBufferSize = indices.size() * sizeof(uint32_t);

	MeshBuffers meshBuffer;

	//Create GPU buffers
	meshBuffer.vertexBuffer = CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);

	VkBufferDeviceAddressInfo deviceAddressInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .buffer = meshBuffer.vertexBuffer.buffer };
	meshBuffer.vertexBufferAddress = vkGetBufferDeviceAddress(device, &deviceAddressInfo);

	meshBuffer.indexBuffer = CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);


	//Copy vert and index buffers to GPU buffers
	VmaBuffer stagingBuffer = CreateBuffer(vertexBufferSize + indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);

	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	vmaCopyMemoryToAllocation(vmaAllocator, vertices.data(), stagingBuffer.allocation, 0, vertexBufferSize);
	vmaCopyMemoryToAllocation(vmaAllocator, indices.data(), stagingBuffer.allocation, vertexBufferSize, indexBufferSize);

	VkBufferCopy vertexCopy{ 0 };
	vertexCopy.dstOffset = 0;
	vertexCopy.srcOffset = 0;
	vertexCopy.size = vertexBufferSize;

	vkCmdCopyBuffer(commandBuffer, stagingBuffer.buffer, meshBuffer.vertexBuffer.buffer, 1, &vertexCopy);

	VkBufferCopy indexCopy{ 0 };
	indexCopy.dstOffset = 0;
	indexCopy.srcOffset = vertexBufferSize;
	indexCopy.size = indexBufferSize;

	vkCmdCopyBuffer(commandBuffer, stagingBuffer.buffer, meshBuffer.indexBuffer.buffer, 1, &indexCopy);

	EndSingleTimeCommands(commandBuffer);

	DestroyBuffer(stagingBuffer);

	return meshBuffer;
}
