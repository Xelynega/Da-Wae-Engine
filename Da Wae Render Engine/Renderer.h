#pragma once

#include "Globals.h"
#define GLFW_INCLUDE_VULKAN
#include "GLFW\glfw3.h"
#include <iostream>

#pragma comment(lib, "vulkan-1.lib")
#pragma comment(lib, "glfw3.lib")

#ifdef _DEBUG
VkBool32 debug = true;
#else
VkBool32 debug = false;
#endif

struct QueueFamily
{
	QueueFamily(int i, uint32_t num) : index(i), numQueues(num) {}
	int index;
	uint32_t numQueues;
};

class Renderer
{
public:
	Renderer(glm::vec4 clear) : clearColor(clear) {};
	byte initGLFW();
	byte splashScreen();
	byte initWindow();
	byte initVulkan();
	byte createInstance();
	byte setupDebugCallback();
	byte createSurface();
	byte pickPhysicalDevice();
	byte createLogicalDevice();
	byte createSwapchain();
	byte createImageViews();
	byte createRenderPass();
	byte createFramebuffers();
	byte createCommandPool();
	byte createSemaphores();
	byte createDescriptorPool();
	byte createCommandBuffers();
	byte copyToBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize dstOffset)
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandBufferCount = 1;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		
		VkCommandBuffer transferBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &transferBuffer);

		VkBufferCopy copyRegion = {};
		copyRegion.dstOffset = dstOffset;
		copyRegion.srcOffset = 0;
		copyRegion.size = size;
		vkCmdCopyBuffer(transferBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		vkEndCommandBuffer(transferBuffer);
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &transferBuffer;

		vkQueueSubmit(graphicsQueues[0], 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueues[0]);
		vkFreeCommandBuffers(device, commandPool, 1, &transferBuffer);
	}

	GLFWwindow* window = nullptr;
	
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	const std::vector<char*> instanceLayers = { "VK_LAYER_LUNARG_core_validation", "VK_LAYER_LUNARG_object_tracker", "VK_LAYER_LUNARG_parameter_validation" };

	std::vector<QueueFamily> queueIndices;
	VkInstance instance;
	VkDebugReportCallbackEXT callback;
	VkDevice device;
	VkPhysicalDevice gpu;
	std::vector<VkQueue> graphicsQueues;
	VkQueue presentQueue;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapchain;
	std::vector<VkImage> swapchainImages;
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;
	std::vector<VkImageView> swapchainImageViews;
	std::vector<VkFramebuffer> swapchainFramebuffers;
	std::vector<VkCommandBuffer> commandBuffers;
	VkCommandPool commandPool;
	VkDescriptorPool descriptorPool;
	VkSurfaceFormatKHR surfaceFormat;
	VkRenderPass renderPass;
	glm::vec4 clearColor;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
};

byte Renderer::initGLFW()
{
	if (glfwInit() != GLFW_TRUE)
		return 0x01;
	return 0x00;
}

byte Renderer::splashScreen()
{
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

	window = glfwCreateWindow(400, 200, "splashscreen", NULL, NULL);
	if (window == nullptr)
		return 0x01;
	return 0x00;
}

byte Renderer::initWindow()
{	
	if(window != nullptr)
		glfwDestroyWindow(window);
	if (!glfwVulkanSupported())
		return 0x01;

	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = glfwCreateWindow(1280, 720, "dawae", NULL, NULL);
	if (window == nullptr)
		return 0x02;
	return 0x00;
}

byte Renderer::initVulkan()
{
	byte value = createInstance();
	if (value != 0x00)
		return value;

	value = setupDebugCallback();
	if (value != 0x00)
		return value;

	value = createSurface();
	if (value != 0x00)
		return value;

	value = pickPhysicalDevice();
	if (value != 0x00)
		return value;

	value = createLogicalDevice();
	if (value != 0x00)
		return value;

	value = createSwapchain();
	if (value != 0x00)
		return value;

	value = createImageViews();
	if (value != 0x00)
		return value;

	value = createRenderPass();
	if (value != 0x00)
		return value;

	value = createFramebuffers();
	if (value != 0x00)
		return value;

	value = createCommandPool();
	if (value != 0x00)
		return value;

	value = createDescriptorPool();
	if (value != 0x00)
		return value;

	value = createSemaphores();
	if (value != 0x00)
		return value;

	value = createCommandBuffers();
	if (value != 0x00)
		return value;
}

byte Renderer::createInstance()
{	
	std::vector<const char*> extensions = {};

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.apiVersion = VK_API_VERSION_1_0;
	appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
	appInfo.pEngineName = "virturoom";
	appInfo.pNext = nullptr;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	if (debug)
	{
		createInfo.enabledLayerCount = instanceLayers.size();
		createInfo.ppEnabledLayerNames = instanceLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
	}

	uint32_t glfwExtensionCount = 0;
	glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	for (int i = 0; i < glfwExtensionCount; i++)
		extensions.push_back(glfwExtensions[i]);
	if (debug)
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	createInfo.enabledExtensionCount = extensions.size();
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.pApplicationInfo = &appInfo;

	if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		return 0x01;
	return 0x00;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData)
{
	std::cerr << msg << std::endl;
	return VK_FALSE;
}

byte Renderer::setupDebugCallback()
{
	if (!debug)
		return 0x00;
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.pUserData = nullptr;
	createInfo.pfnCallback = debugCallback;
	createInfo.pNext = nullptr;

	auto createCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	createCallback(instance, &createInfo, nullptr, &callback);
	return 0x00;
}

byte Renderer::createSurface()
{
	glfwCreateWindowSurface(instance, window, nullptr, &surface);
	return 0x00;
}

byte Renderer::pickPhysicalDevice()
{
	uint32_t gpuCount = 0;
	vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);

	VkPhysicalDevice* gpus = new VkPhysicalDevice[gpuCount];
	vkEnumeratePhysicalDevices(instance, &gpuCount, gpus);

	int* scores = new int[gpuCount];

	for (int i = 0; i < gpuCount; i++)
	{
		scores[i] = 0;
		VkPhysicalDeviceProperties gpuProperties;
		vkGetPhysicalDeviceProperties(gpus[i], &gpuProperties);
		switch (gpuProperties.deviceType)
		{
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			scores[i] += 10;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			scores[i] += 250;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			scores[i] += 50;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			scores[i] += 100;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			scores[i] += 20;
			break;
		}
	}
	int maxIndex = 0;
	int max = 0;
	for (int i = 0; i < gpuCount; i++)
	{
		if (scores[i] > max)
		{
			max = scores[i];
			maxIndex = i;
		}
	}
	gpu = gpus[maxIndex];
	return 0x00;
}

std::vector<QueueFamily> findGPUIndices(VkPhysicalDevice gpu, VkSurfaceKHR surface)
{
	std::vector<QueueFamily> queues;
	queues.push_back(QueueFamily( -1, 0 ));

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, nullptr);

	VkQueueFamilyProperties* properties = new VkQueueFamilyProperties[queueFamilyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, properties);

	for (int i = 0; i < queueFamilyCount; i++)
	{
		if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
				queues.push_back(QueueFamily( i, properties[i].queueCount ));
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &presentSupport);

		if (presentSupport && queues[0].index == -1)
		{
			queues[0] = QueueFamily( i, properties[i].queueCount );
		}
	}
	return queues;
}

byte Renderer::createLogicalDevice()
{
	queueIndices = findGPUIndices(gpu, surface);

	VkPhysicalDeviceFeatures deviceFeatures = {};
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.flags = 0;
	createInfo.enabledExtensionCount = deviceExtensions.size();
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	if (debug)
	{
		createInfo.enabledLayerCount = instanceLayers.size();
		createInfo.ppEnabledLayerNames = instanceLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (queueIndices[0].index == queueIndices[1].index)
	{
		float* priorities = new float[queueIndices[0].numQueues];
		for (int i = 0; i < queueIndices[0].numQueues; i++)
			priorities[i] = 1.0f;
		VkDeviceQueueCreateInfo queueInfo = {};
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.flags = 0;
		queueInfo.pQueuePriorities = priorities;
		queueInfo.queueCount = queueIndices[0].numQueues;
		queueInfo.queueFamilyIndex = queueIndices[0].index;
		queueInfo.pNext = NULL;

		createInfo.pQueueCreateInfos = &queueInfo;
		createInfo.queueCreateInfoCount = 1;
	}
	else
	{
		float presentPriority = 1.0f;

		float* graphicsPriorities = new float[queueIndices[1].numQueues];
		for (int i = 0; i < queueIndices[1].numQueues; i++)
			graphicsPriorities[i] = 1.0f;
		VkDeviceQueueCreateInfo queueInfo[2];
		queueInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo[0].flags = 0;
		queueInfo[0].pQueuePriorities = &presentPriority;
		queueInfo[0].queueCount = 1;
		queueInfo[0].queueFamilyIndex = queueIndices[0].index;
		queueInfo[0].pNext = NULL;

		queueInfo[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo[1].flags = 0;
		queueInfo[1].pQueuePriorities = graphicsPriorities;
		queueInfo[1].queueCount = queueIndices[1].numQueues;
		queueInfo[1].queueFamilyIndex = queueIndices[1].index;
		queueInfo[1].pNext = NULL;

		createInfo.pQueueCreateInfos = queueInfo;
		createInfo.queueCreateInfoCount = 2;
	}

	vkCreateDevice(gpu, &createInfo, nullptr, &device);

	if (queueIndices[0].index == queueIndices[1].index)
	{
		vkGetDeviceQueue(device, queueIndices[0].index, 0, &presentQueue);
		graphicsQueues.resize(queueIndices[0].numQueues - 1);

		graphicsQueues.resize(queueIndices[0].numQueues-1);

		for (int i = 1; i < queueIndices[0].numQueues; i++)
		{
			vkGetDeviceQueue(device, queueIndices[0].index, i, &graphicsQueues[i-1]);
		}
	}
	else
	{
		vkGetDeviceQueue(device, queueIndices[0].index, 0, &presentQueue);

		graphicsQueues.resize(queueIndices[1].numQueues);
		for (int i = 0; i < queueIndices[1].numQueues; i++)
		{
			vkGetDeviceQueue(device, queueIndices[1].index, i, &graphicsQueues[i]);
		}
	}

	return 0x00;
}

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	details.formats.resize(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());

	uint32_t modeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &modeCount, nullptr);
	details.presentModes.resize(modeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &modeCount, details.presentModes.data());

	return details;
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}
	for (const VkSurfaceFormatKHR& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;
	}
	return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const VkPresentModeKHR& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return availablePresentMode;
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			bestMode = availablePresentMode;
	}

	return bestMode;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
{
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else
	{
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		VkExtent2D actualExtent = { width, height };

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
	}
}

byte Renderer::createSwapchain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(gpu, surface);

	surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);

	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);

	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapInfo = {};
	swapInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapInfo.surface = surface;

	swapInfo.minImageCount = imageCount;
	swapInfo.imageFormat = surfaceFormat.format;
	swapInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapInfo.imageExtent = extent;
	swapInfo.imageArrayLayers = 1;
	swapInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	uint32_t queueFamilyIndices[] = { (uint32_t)queueIndices[0].index, (uint32_t)queueIndices[1].index };
	if (queueFamilyIndices[0] != queueFamilyIndices[1])
	{
		swapInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapInfo.queueFamilyIndexCount = 2;
		swapInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		swapInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	swapInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	swapInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapInfo.presentMode = presentMode;
	swapInfo.clipped = VK_TRUE;
	swapInfo.oldSwapchain = VK_NULL_HANDLE;

	vkCreateSwapchainKHR(device, &swapInfo, nullptr, &swapchain);

	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
	swapchainImages.resize(imageCount);
	commandBuffers.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());

	swapchainImageFormat = surfaceFormat.format;
	swapchainExtent = extent;
	return 0x00;
}

byte Renderer::createImageViews()
{
	swapchainImageViews.resize(swapchainImages.size());

	for (uint32_t index = 0; index < swapchainImages.size(); index++)
	{
		VkImageViewCreateInfo view_info = {};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = swapchainImages[index];
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = swapchainImageFormat;

		view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;

		vkCreateImageView(device, &view_info, nullptr, &swapchainImageViews[index]);
	}
	return 0x00;
}

byte Renderer::createRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapchainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
	return 0x00;
}

byte Renderer::createFramebuffers()
{
	swapchainFramebuffers.resize(swapchainImageViews.size());

	for (uint32_t index = 0; index < swapchainImageViews.size(); index++)
	{
		VkImageView attachments[] = { swapchainImageViews[index] };

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapchainExtent.width;
		framebufferInfo.height = swapchainExtent.height;
		framebufferInfo.layers = 1;

		vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapchainFramebuffers[index]);
	}
	return 0x00;
}

byte Renderer::createCommandPool()
{
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueIndices[1].index;
	poolInfo.flags = 0;

	vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
	return 0x00;
}

byte Renderer::createSemaphores()
{
	VkSemaphoreCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	vkCreateSemaphore(device, &createInfo, nullptr, &imageAvailableSemaphore);
	vkCreateSemaphore(device, &createInfo, nullptr, &renderFinishedSemaphore);

	return 0x00;
}

byte Renderer::createDescriptorPool()
{
	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = DESCRIPTOR_SET_COUNT;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = DESCRIPTOR_SET_COUNT;

	vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool);
	return 0x00;
}

byte Renderer::createCommandBuffers()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data());

	VkClearColorValue vkclearColor = { {clearColor.r, clearColor.g, clearColor.b, clearColor.a} };

	VkImageSubresourceRange subResourceRange = {};
	subResourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subResourceRange.baseMipLevel = 0;
	subResourceRange.levelCount = 1;
	subResourceRange.baseArrayLayer = 0;
	subResourceRange.layerCount = 1;

	for (uint32_t index = 0; index < commandBuffers.size();index++)
	{
		VkImageMemoryBarrier presentToClearBarrier = {};
		presentToClearBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		presentToClearBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		presentToClearBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		presentToClearBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		presentToClearBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		presentToClearBarrier.srcQueueFamilyIndex = queueIndices[0].index;
		presentToClearBarrier.dstQueueFamilyIndex = queueIndices[0].index;
		presentToClearBarrier.image = swapchainImages[index];
		presentToClearBarrier.subresourceRange = subResourceRange;

		VkImageMemoryBarrier clearToPresentBarrier = {};
		clearToPresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		clearToPresentBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		clearToPresentBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		clearToPresentBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		clearToPresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		clearToPresentBarrier.srcQueueFamilyIndex = queueIndices[0].index;
		clearToPresentBarrier.dstQueueFamilyIndex = queueIndices[0].index;
		clearToPresentBarrier.image = swapchainImages[index];
		clearToPresentBarrier.subresourceRange = subResourceRange;

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr;

		vkBeginCommandBuffer(commandBuffers[index], &beginInfo);

		vkCmdPipelineBarrier(commandBuffers[index], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &presentToClearBarrier);

		vkCmdClearColorImage(commandBuffers[index], swapchainImages[index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &vkclearColor, 1, &subResourceRange);

		vkCmdPipelineBarrier(commandBuffers[index], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &clearToPresentBarrier);

		VkResult result = vkEndCommandBuffer(commandBuffers[index]);
		if (result != VK_SUCCESS)
			return 0x01;
	}
	return 0x00;
}