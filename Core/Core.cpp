#include "SckVK_Core.h"
#include <vulkan/vulkan.h>

namespace sckVK
{
	VulkanCore::VulkanCore()
	{
	}

	VulkanCore::~VulkanCore()
	{
		vkDestroyCommandPool(m_device, m_VkCommandPool, nullptr);
		printf("Command Buffer Pool Destroyed\n");

		for (uint32_t i = 0; i < m_Images.size(); i++)
		{
			vkDestroyImageView(m_device, m_ImageViews[i], nullptr);
		}
		printf("Image Views Destroyed\n");

		vkDestroySwapchainKHR(m_device, m_Swapchain, nullptr);
		printf("Swapchain Destroyed\n");

		vkDestroyDevice(m_device, nullptr);
		printf("Logical Device Destroyed\n");

		PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR = VK_NULL_HANDLE;
		vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)vkGetInstanceProcAddr(m_VkInstance, "vkDestroySurfaceKHR");
		if (!vkDestroySurfaceKHR)
		{
			printf("Cannot find address of vkDestroySurfaceKHR\n");
			exit(1);
		}
		vkDestroySurfaceKHR(m_VkInstance, m_VkSurface, nullptr);
		printf("GLFW Window Surface Destroyed\n");

		PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = VK_NULL_HANDLE;
		vkDestroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VkInstance, "vkDestroyDebugUtilsMessengerEXT");
		if (!vkDestroyDebugUtilsMessenger)
		{
			printf("Cannot find address of vkDestroyDebugUtilsMessengerEXT\n");
			exit(1);
		}
		vkDestroyDebugUtilsMessenger(m_VkInstance, m_VkDebugMessenger, nullptr);
		printf("Debug Utils Messenger Destroyed\n");

		vkDestroyInstance(m_VkInstance, nullptr);
		printf("Vulkan Instance Destroyed\n");
	}

	void VulkanCore::Init(const char* appName, GLFWwindow* window)
	{
		CreateInstance(appName);
		CreateDebugCallback();
		CreateSurface(window);
		m_VulkanPhysicalDevices.Init(m_VkInstance, m_VkSurface);
		m_queueFamily = m_VulkanPhysicalDevices.SelectDevice(VK_QUEUE_GRAPHICS_BIT, true);
		CreateDevice();
		CreateSwapchain();
		CreateCommandBufferPool();
		m_vkQueue.Init(m_device, m_Swapchain, m_queueFamily, 0);
	}

	uint32_t VulkanCore::GetSwapchainImageCount()
	{
		return m_swapchainImageCount;
	}

	VkImage& VulkanCore::GetImage(uint32_t imgNumber)
	{
		return m_Images[imgNumber];
	}

	VulkanQueue* VulkanCore::GetQueue()
	{
		return &m_vkQueue;
	}

	void VulkanCore::CreateCommandBuffers(uint32_t count, VkCommandBuffer* cmdBuffers)
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = m_VkCommandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = count
		};

		VkResult res = vkAllocateCommandBuffers(m_device, &commandBufferAllocateInfo, cmdBuffers);
		CHECK_VK_RESULT(res, "vkAllocateCommandBuffers error\n");

		printf("Command Buffers Created\n");
	}

	void VulkanCore::FreeCommandBuffers(uint32_t count, VkCommandBuffer* cmdBuffers)
	{
		vkFreeCommandBuffers(m_device, m_VkCommandPool, count, cmdBuffers);
		printf("Command Buffers Destroyed\n");
	}

	void VulkanCore::CreateInstance(const char* appName)
	{
		std::vector<const char*> layers = {
			"VK_LAYER_KHRONOS_validation"
		};

		std::vector<const char*> extensions = {
			VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(_WIN32)
			"VK_KHR_win32_surface",
#endif
#if defined(__APPLE__)
			"VK_MVK_macos_surface",
#endif
#if defined(__linux__)
			"VK_KHR_xcb_surface",
#endif
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME
		};

		VkApplicationInfo appInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = appName,
			.applicationVersion = VK_MAKE_API_VERSION(0,1,0,0),
			.pEngineName = appName,
			.engineVersion = VK_MAKE_API_VERSION(0,1,0,0),
			.apiVersion = VK_API_VERSION_1_0
		};

		VkInstanceCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.pApplicationInfo = &appInfo,
			.enabledLayerCount = (uint32_t)(layers.size()),
			.ppEnabledLayerNames = layers.data(),
			.enabledExtensionCount = (uint32_t)(extensions.size()),
			.ppEnabledExtensionNames = extensions.data()
		};

		VkResult res = vkCreateInstance(&createInfo, nullptr, &m_VkInstance);
		CHECK_VK_RESULT(res, "Create Instance");
		printf("Vulkan Instance Created\n");
	}

	static VKAPI_ATTR VkBool32 VKAPI_PTR DebugMessengerCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
		const VkDebugUtilsMessengerCallbackDataEXT*		 pCallbackData,
		void*											 pUserData)
	{
		printf("Debug Callback : %s\n", pCallbackData->pMessage);
		printf("Severity : %s\n", MessageSeverity(messageSeverity));
		printf("Type : %s\n", MessageType(messageTypes));
		printf(" Objects ");

		for (uint32_t i = 0; i < pCallbackData->objectCount; i++)
		{
			printf("%llx\n", pCallbackData->pObjects[i].objectHandle);
		}

		printf("\n");

		return VK_FALSE; // The calling function should not be aborted
	}

	void VulkanCore::CreateDebugCallback()
	{
		VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.pNext = nullptr,
			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
							   VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
							   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
							   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
						   VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
						   VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			.pfnUserCallback = &DebugMessengerCallback,
			.pUserData = nullptr
		};

		PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger = VK_NULL_HANDLE;
		vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VkInstance, "vkCreateDebugUtilsMessengerEXT");
		if (!vkCreateDebugUtilsMessenger)
		{
			printf("Cannot find address of vkCreateDebugUtilsMessengerEXT\n");
			exit(1);
		}

		VkResult res = vkCreateDebugUtilsMessenger(m_VkInstance, &messengerCreateInfo, nullptr, &m_VkDebugMessenger);
		CHECK_VK_RESULT(res, "Debug Utils Messenger");

		printf("Debug Utils Messenger Created\n");
	}

	void VulkanCore::CreateSurface(GLFWwindow* window)
	{
		VkResult res = glfwCreateWindowSurface(m_VkInstance, window, nullptr, &m_VkSurface);
		CHECK_VK_RESULT(res, "GLFW Window Surface");

		printf("GLFW Window Surface Created\n");
	}

	void VulkanCore::CreateDevice()
	{
		float priorities[] = { 1.0f };

		VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueFamilyIndex = m_queueFamily,
			.queueCount = 1,
			.pQueuePriorities = &priorities[0]
		};

		std::vector<const char*> extensionNames = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME
		};

		if (!(m_VulkanPhysicalDevices.SelectedDevice().m_physicalDeviceFeatures.geometryShader == VK_TRUE))
		{
			printf("Geometry shader is not enabled!");
			exit(1);
		}
		
		if (!(m_VulkanPhysicalDevices.SelectedDevice().m_physicalDeviceFeatures.tessellationShader == VK_TRUE))
		{
			printf("Tessellation shader is not enabled!");
			exit(1);
		}

		VkPhysicalDeviceFeatures physicalDeviceFeatures = { 0 };
		physicalDeviceFeatures.geometryShader = VK_TRUE;
		physicalDeviceFeatures.tessellationShader = VK_TRUE;

		VkDeviceCreateInfo deviceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = &deviceQueueCreateInfo,
			.enabledLayerCount = 0,			// DEPRECATED
			.ppEnabledLayerNames = nullptr,	// DEPRECATED
			.enabledExtensionCount = (uint32_t)extensionNames.size(),
			.ppEnabledExtensionNames = extensionNames.data(),
			.pEnabledFeatures = &physicalDeviceFeatures
		};

		VkResult res = vkCreateDevice(m_VulkanPhysicalDevices.SelectedDevice().m_physicalDevice, &deviceCreateInfo, nullptr, &m_device);
		CHECK_VK_RESULT(res, "vkCreateDevice error\n");

		printf("Logical Device Created\n");
	}

	static VkSurfaceFormatKHR ChooseSurfaceFormatAndColorSpace(const std::vector<VkSurfaceFormatKHR>& surfaceFormats)
	{
		for (uint32_t i = 0; i < surfaceFormats.size(); i++)
		{
			if ((surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) &&
				(surfaceFormats[i].format == VK_FORMAT_R8G8B8A8_SRGB))
			{
				return surfaceFormats[i];
			}
		}

		return surfaceFormats[0];
	}

	static VkPresentModeKHR ChoosePresentationMode(const std::vector<VkPresentModeKHR>& presentationModes)
	{
		for (uint32_t i = 0; i < presentationModes.size(); i++)
		{
			if (presentationModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return presentationModes[i];
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	static uint32_t SelectNumberOfImages(const VkSurfaceCapabilitiesKHR& surfaceCaps)
	{
		uint32_t NumImg = surfaceCaps.minImageCount + 1;

		if ((NumImg < surfaceCaps.maxImageCount) && (surfaceCaps.maxImageCount > 0))
		{
			return NumImg;
		}
		else
		{
			return surfaceCaps.maxImageCount;
		}
	}

	static VkImageView CreateImageView(VkDevice device, VkImage image, VkImageViewType viewType, VkFormat format,
										VkImageAspectFlags aspectFlags, uint32_t levelCount, uint32_t layerCount)
	{
		VkImageView imageView;

		VkImageViewCreateInfo imageViewCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.image = image,
			.viewType = viewType,
			.format = format,
			.components = {
				.r = VK_COMPONENT_SWIZZLE_R,
				.g = VK_COMPONENT_SWIZZLE_G,
				.b = VK_COMPONENT_SWIZZLE_B,
				.a = VK_COMPONENT_SWIZZLE_A
			},
			.subresourceRange = {
				.aspectMask = aspectFlags,
				.baseMipLevel = 0,
				.levelCount = levelCount,
				.baseArrayLayer = 0,
				.layerCount = layerCount
			}
		};

		VkResult res = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageView);
		CHECK_VK_RESULT(res, "vkCreateImageView error\n");

		return imageView;
	}

	void VulkanCore::CreateSwapchain()
	{
		VkSurfaceCapabilitiesKHR surfaceCaps = m_VulkanPhysicalDevices.SelectedDevice().m_surfaceCapabilities;
		m_swapchainImageCount = SelectNumberOfImages(surfaceCaps);

		VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormatAndColorSpace(m_VulkanPhysicalDevices.SelectedDevice().m_surfaceFormats);

		VkPresentModeKHR presentMode = ChoosePresentationMode(m_VulkanPhysicalDevices.SelectedDevice().m_presentModes);

		VkSwapchainCreateInfoKHR swapchainCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.pNext = nullptr,
			.flags = 0,
			.surface = m_VkSurface,
			.minImageCount = m_swapchainImageCount,
			.imageFormat = surfaceFormat.format,
			.imageColorSpace = surfaceFormat.colorSpace,
			.imageExtent = surfaceCaps.currentExtent,
			.imageArrayLayers = 1,
			.imageUsage = (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT),
			.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 1,
			.pQueueFamilyIndices = &m_queueFamily,
			.preTransform = surfaceCaps.currentTransform,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = presentMode,
			.clipped = VK_TRUE
		};

		VkResult res = vkCreateSwapchainKHR(m_device, &swapchainCreateInfo, nullptr, &m_Swapchain);
		CHECK_VK_RESULT(res, "vkCreateSwapchainKHR error\n");

		printf("Swapchain Created\n");

		uint32_t SwapchainImageNumber = 0;
		res = vkGetSwapchainImagesKHR(m_device, m_Swapchain, &SwapchainImageNumber, nullptr);
		CHECK_VK_RESULT(res, "vkGetSwapchainImagesKHR error\n");

		if (m_swapchainImageCount != SwapchainImageNumber)
		{
			printf("Swapchain images could not be allocated\n");
			exit(1);
		}

		m_Images.resize(SwapchainImageNumber);
		m_ImageViews.resize(SwapchainImageNumber);

		res = vkGetSwapchainImagesKHR(m_device, m_Swapchain, &SwapchainImageNumber, m_Images.data());
		CHECK_VK_RESULT(res, "vkGetSwapchainImagesKHR error\n");

		uint32_t mipLevel = 1;
		uint32_t layerCount = 1;
		for (uint32_t i = 0; i < m_Images.size(); i++)
		{
			m_ImageViews[i] = CreateImageView(m_device, m_Images[i], VK_IMAGE_VIEW_TYPE_2D, surfaceFormat.format,
												VK_IMAGE_ASPECT_COLOR_BIT, mipLevel, layerCount);
		}
		printf("Image Views Created\n");
	}

	void VulkanCore::CreateCommandBufferPool()
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueFamilyIndex = m_queueFamily
		};

		VkResult res = vkCreateCommandPool(m_device, &commandPoolCreateInfo, nullptr, &m_VkCommandPool);
		CHECK_VK_RESULT(res, "vkCreateCommandPool error\n");

		printf("Command Buffer Pool Created\n");
	}
}