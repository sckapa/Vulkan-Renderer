#include "SckVK_Core.h"
#include <vulkan/vulkan.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image/stb_image.h"

namespace sckVK
{
	VulkanCore::VulkanCore()
	{
	}

	VulkanCore::~VulkanCore()
	{
		m_vkQueue.DestroySemaphores();
		printf("Semaphores Destroyed\n");

		vkFreeCommandBuffers(m_device, m_VkCommandPool, 1, &m_copyCommandBuffer);

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
		glfwGetWindowSize(window, &m_windowWidth, &m_windowHeight);

		CreateInstance(appName);
		CreateDebugCallback();
		CreateSurface(window);
		m_VulkanPhysicalDevices.Init(m_VkInstance, m_VkSurface);
		m_queueFamily = m_VulkanPhysicalDevices.SelectDevice(VK_QUEUE_GRAPHICS_BIT, true);
		CreateDevice();
		CreateSwapchain();
		CreateCommandBufferPool();
		m_vkQueue.Init(m_device, m_Swapchain, m_queueFamily, 0);
		CreateCommandBuffers(1, &m_copyCommandBuffer);
		CreateDepthImages();
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

	VkDevice VulkanCore::GetDevice()
	{
		return m_device;
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
		m_vkQueue.WaitIdle();
		vkFreeCommandBuffers(m_device, m_VkCommandPool, count, cmdBuffers);
	}

	VkRenderPass VulkanCore::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachmentDesc = {
			.flags = 0,
			.format = m_swapchainSurfaceFormat.format,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		};
		
		VkAttachmentDescription depthAttachmentDesc = {
			.flags = 0,
			.format = m_VulkanPhysicalDevices.SelectedDevice().m_depthFormat,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};

		std::vector< VkAttachmentDescription> attachmentDescriptions;
		attachmentDescriptions.push_back(colorAttachmentDesc);
		attachmentDescriptions.push_back(depthAttachmentDesc);

		VkAttachmentReference colorAttachmentReference = {
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		};
		
		VkAttachmentReference depthAttachmentReference = {
			.attachment = 1,
			.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};

		VkSubpassDescription subpassDescription = {
			.flags = 0,
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.inputAttachmentCount = 0,
			.pInputAttachments = nullptr,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachmentReference,
			.pResolveAttachments = nullptr,
			.pDepthStencilAttachment = &depthAttachmentReference,
			.preserveAttachmentCount = 0,
			.pPreserveAttachments = nullptr
		};

		VkRenderPassCreateInfo renderPassCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.attachmentCount = (uint32_t)attachmentDescriptions.size(),
			.pAttachments = attachmentDescriptions.data(),
			.subpassCount = 1,
			.pSubpasses = &subpassDescription,
			.dependencyCount = 0,
			.pDependencies = nullptr
		};

		VkRenderPass RenderPass;

		VkResult res = vkCreateRenderPass(m_device, &renderPassCreateInfo, nullptr, &RenderPass);
		CHECK_VK_RESULT(res, "vkCreateRenderPass error\n");

		printf("Render Pass Created\n");

		return RenderPass;
	}

	std::vector<VkFramebuffer> VulkanCore::CreateFrameBuffers(VkRenderPass renderPass)
	{
		m_frameBuffers.resize(m_ImageViews.size());

		VkResult res;

		for (uint32_t i = 0; i < m_ImageViews.size(); i++)
		{
			std::vector<VkImageView> attachments;
			attachments.push_back(m_ImageViews[i]);
			attachments.push_back(m_depthImages[i].m_imageView);

			VkFramebufferCreateInfo framebufferCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.renderPass = renderPass,
				.attachmentCount = (uint32_t)attachments.size(),
				.pAttachments = attachments.data(),
				.width = (uint32_t)m_windowWidth,
				.height = (uint32_t)m_windowHeight,
				.layers = 1
			};

			res = vkCreateFramebuffer(m_device, &framebufferCreateInfo, nullptr, &m_frameBuffers[i]);
			CHECK_VK_RESULT(res, "vkCreateFramebuffer error\n");
		}

		printf("Frame Buffers created\n");

		return m_frameBuffers;
	}

	void VulkanCore::DestroyFrameBuffers()
	{
		for (uint32_t i = 0; i < m_frameBuffers.size(); i++)
		{
			vkDestroyFramebuffer(m_device, m_frameBuffers[i], nullptr);
		}
	}

	BufferAndMemory VulkanCore::CreateVertexBuffer(const void* vertices, size_t size)
	{
		VkBufferUsageFlags bufferUsageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		BufferAndMemory stagingVertexBuffer = CreateBuffer(size, bufferUsageFlags, memoryPropertyFlags);

		void* mem;
		VkDeviceSize offset = 0;
		VkMemoryMapFlags flags = 0;
		vkMapMemory(m_device, stagingVertexBuffer.m_memory, offset, stagingVertexBuffer.m_allocationSize, flags, &mem);

		memcpy(mem, vertices, size);

		vkUnmapMemory(m_device, stagingVertexBuffer.m_memory);

		VkBufferUsageFlags bufferUsageFlagsVB = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		VkMemoryPropertyFlags memoryPropertyFlagsVB = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		BufferAndMemory vertexBuffer = CreateBuffer(size, bufferUsageFlagsVB, memoryPropertyFlagsVB);

		CopyBuffer(stagingVertexBuffer.m_buffer, vertexBuffer.m_buffer, size);

		stagingVertexBuffer.Destroy(m_device);

		return vertexBuffer;
	}

	void VulkanCore::CreateTexture(const char* filePath, VulkanTexture& texture)
	{
		int imageWidth = 0;
		int imageHeight = 0;
		int imageChannels = 0;

		stbi_uc* pixelData = stbi_load(filePath, &imageWidth, &imageHeight, &imageChannels, STBI_rgb_alpha);

		if (!pixelData)
		{
			printf("Could not read file %s\n", filePath);
			exit(1);
		}

		VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
		CreateTextureImageFromData(texture, pixelData, imageWidth, imageHeight, format);

		stbi_image_free(pixelData);

		VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
		texture.m_imageView = sckVK::CreateImageView(m_device, texture.m_image, format, aspectFlags);

		VkFilter magFilter = VK_FILTER_LINEAR;
		VkFilter minFilter = VK_FILTER_LINEAR;
		VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		texture.m_sampler = CreateTextureSampler(magFilter, minFilter, addressMode);
	}

	std::vector<BufferAndMemory> VulkanCore::CreateUniformBuffers(size_t size)
	{
		std::vector<BufferAndMemory> UniformBuffers;

		UniformBuffers.resize(m_Images.size());

		for (int i = 0; i < UniformBuffers.size(); i++)
		{
			UniformBuffers[i] = CreateUniformBuffer(size);
		}

		return UniformBuffers;
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

	void VulkanCore::CreateSwapchain()
	{
		VkSurfaceCapabilitiesKHR surfaceCaps = m_VulkanPhysicalDevices.SelectedDevice().m_surfaceCapabilities;
		m_swapchainImageCount = SelectNumberOfImages(surfaceCaps);

		m_swapchainSurfaceFormat = ChooseSurfaceFormatAndColorSpace(m_VulkanPhysicalDevices.SelectedDevice().m_surfaceFormats);

		VkPresentModeKHR presentMode = ChoosePresentationMode(m_VulkanPhysicalDevices.SelectedDevice().m_presentModes);

		VkSwapchainCreateInfoKHR swapchainCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.pNext = nullptr,
			.flags = 0,
			.surface = m_VkSurface,
			.minImageCount = m_swapchainImageCount,
			.imageFormat = m_swapchainSurfaceFormat.format,
			.imageColorSpace = m_swapchainSurfaceFormat.colorSpace,
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
			m_ImageViews[i] = sckVK::CreateImageView(m_device, m_Images[i], m_swapchainSurfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
		}
		printf("Image Views Created\n");
	}

	void VulkanCore::CreateCommandBufferPool()
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = m_queueFamily
		};

		VkResult res = vkCreateCommandPool(m_device, &commandPoolCreateInfo, nullptr, &m_VkCommandPool);
		CHECK_VK_RESULT(res, "vkCreateCommandPool error\n");

		printf("Command Buffer Pool Created\n");
	}

	void VulkanCore::SubmitCopyBuffer()
	{
		vkEndCommandBuffer(m_copyCommandBuffer);

		m_vkQueue.SubmitSync(m_copyCommandBuffer);

		m_vkQueue.WaitIdle();
	}

	void VulkanCore::CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size)
	{
		BeginCommandBuffer(m_copyCommandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		VkBufferCopy bufferCopy = {
			.srcOffset = 0,
			.dstOffset = 0,
			.size = size
		};
		vkCmdCopyBuffer(m_copyCommandBuffer, src, dst, 1, &bufferCopy);

		SubmitCopyBuffer();
	}

	BufferAndMemory VulkanCore::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryPropertyFlags)
	{
		BufferAndMemory buffer;

		VkBufferCreateInfo bufferCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = size,
			.usage = usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE
		};

		VkResult res = vkCreateBuffer(m_device, &bufferCreateInfo, nullptr, &buffer.m_buffer);
		CHECK_VK_RESULT(res, "vkCreateBuffer error\n");

		VkMemoryRequirements memReq;
		vkGetBufferMemoryRequirements(m_device, buffer.m_buffer, &memReq);

		buffer.m_allocationSize = memReq.size;

		uint32_t memoryTypeIndex = GetMemoryTypeIndex(memReq.memoryTypeBits, memoryPropertyFlags);

		VkMemoryAllocateInfo memoryAllocateInfo = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = memReq.size,
			.memoryTypeIndex = memoryTypeIndex
		};

		res = vkAllocateMemory(m_device, &memoryAllocateInfo, nullptr, &buffer.m_memory);
		CHECK_VK_RESULT(res, "vkAllocateMemory error\n");

		res = vkBindBufferMemory(m_device, buffer.m_buffer, buffer.m_memory, 0);
		CHECK_VK_RESULT(res, "vkBindBufferMemory error\n");

		return buffer;
	}

	BufferAndMemory VulkanCore::CreateUniformBuffer(size_t size)
	{
		VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		VkMemoryPropertyFlags memPropFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

		return CreateBuffer(size, usage, memPropFlags);
	}

	uint32_t VulkanCore::GetMemoryTypeIndex(uint32_t memoryType, VkMemoryPropertyFlags memoryPropertyFlags)
	{
		const VkPhysicalDeviceMemoryProperties& prop = m_VulkanPhysicalDevices.SelectedDevice().m_memoryProps;

		for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
		{
			const VkMemoryType& phyMemProp = prop.memoryTypes[i];
			uint32_t bit = (1 << i);
			bool isCurrMemTypeSupported = (bit & memoryType);
			bool hasRequiredMemProperties = ((phyMemProp.propertyFlags & memoryPropertyFlags) == memoryPropertyFlags);

			if (isCurrMemTypeSupported && hasRequiredMemProperties)
			{
				return i;
			}
		}

		printf("Cannot find memory type index for requested type %x and mem props %x\n", memoryType, memoryPropertyFlags);
		exit(1);
		return -1;
	}

	void VulkanCore::CreateTextureImageFromData(VulkanTexture& texture, const void* data, uint32_t imageWidth, uint32_t imageHeight, VkFormat format)
	{
		VkImageUsageFlags flags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkMemoryPropertyFlags memProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		CreateImage(texture, imageWidth, imageHeight, format, flags, memProps);

		UpdateImage(texture, imageWidth, imageHeight, format, data);
	}

	void VulkanCore::CreateImage(VulkanTexture& texture, uint32_t imageWidth, uint32_t imageHeight, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags memProperties)
	{
		VkImageCreateInfo imageCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.flags = 0,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = format,
			.extent = VkExtent3D {.width = imageWidth, .height = imageHeight, .depth = 1},
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
		};

		VkResult res = vkCreateImage(m_device, &imageCreateInfo, nullptr, &texture.m_image);
		CHECK_VK_RESULT(res, "vkCreateImage error\n");

		VkMemoryRequirements memRequirements = { 0 };
		vkGetImageMemoryRequirements(m_device, texture.m_image, &memRequirements);

		uint32_t memTypeIndex = GetMemoryTypeIndex(memRequirements.memoryTypeBits, memProperties);

		VkMemoryAllocateInfo memAllocateInfo = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = memRequirements.size,
			.memoryTypeIndex = memTypeIndex
		};

		res = vkAllocateMemory(m_device, &memAllocateInfo, nullptr, &texture.m_memory);
		CHECK_VK_RESULT(res, "vkAllocateMemory error\n");

		res = vkBindImageMemory(m_device, texture.m_image, texture.m_memory, 0);
		CHECK_VK_RESULT(res, "vkBindImageMemory error\n");
	}

	void VulkanCore::UpdateImage(VulkanTexture& texture, uint32_t imageWidth, uint32_t imageHeight, VkFormat format, const void* data)
	{
		int bytesPerPixel = GetBytesPerPixel(format);

		VkDeviceSize layerSize = imageWidth * imageHeight * bytesPerPixel;
		int layerCount = 1;

		VkDeviceSize imageSize = layerSize * layerCount;
		VkBufferUsageFlags flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		VkMemoryPropertyFlags memPropFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		BufferAndMemory stagingBuffer = CreateBuffer(imageSize, flags, memPropFlags);

		stagingBuffer.Update(m_device, data, imageSize);

		TransitionImageLayout(texture.m_image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layerCount);

		CopyBufferToImage(stagingBuffer.m_buffer, texture.m_image, imageWidth, imageHeight);

		TransitionImageLayout(texture.m_image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, layerCount);

		stagingBuffer.Destroy(m_device);
	}

	int VulkanCore::GetBytesPerPixel(VkFormat format)
	{
		switch (format)
		{
		case VK_FORMAT_R8_SINT:
			return 1;
		case VK_FORMAT_R8_UNORM:
			return 2;
		case VK_FORMAT_R16_SFLOAT:
			return 4;
		case VK_FORMAT_R16G16_SFLOAT:
			return 4;
		case VK_FORMAT_R16G16_SNORM:
			return 4;
		case VK_FORMAT_B8G8R8A8_UNORM:
			return 4;
		case VK_FORMAT_R8G8B8A8_UNORM:
			return 4;
		case VK_FORMAT_R16G16B16A16_SFLOAT:
			return 4 * sizeof(uint16_t);
		case VK_FORMAT_R32G32B32A32_SFLOAT:
			return 4 * sizeof(float);
		default:
			printf("Unknown format %d\n", format);
			exit(1);
		}

		return 0;
	}

	void VulkanCore::TransitionImageLayout(VkImage& image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, int layerCount)
	{
		BeginCommandBuffer(m_copyCommandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		sckVK::ImageMemBarrier(m_copyCommandBuffer, image, format, oldLayout, newLayout, layerCount);

		SubmitCopyBuffer();
	}

	void VulkanCore::CopyBufferToImage(VkBuffer src, VkImage dst, uint32_t imageWidth, uint32_t imageHeight)
	{
		BeginCommandBuffer(m_copyCommandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		VkBufferImageCopy bufferImageCopy = {
			.bufferOffset = 0,
			.bufferRowLength = 0,
			.bufferImageHeight = 0,
			.imageSubresource = VkImageSubresourceLayers
			{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
			.imageOffset = VkOffset3D {.x = 0,.y = 0,.z = 0},
			.imageExtent = VkExtent3D {.width = imageWidth,.height = imageHeight, .depth = 1}
		};

		vkCmdCopyBufferToImage(m_copyCommandBuffer, src, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopy);

		SubmitCopyBuffer();
	}

	VkSampler VulkanCore::CreateTextureSampler(VkFilter magFilter, VkFilter minFilter, VkSamplerAddressMode addressMode)
	{
		VkSamplerCreateInfo samplerCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.magFilter = magFilter,
			.minFilter = minFilter,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU = addressMode,
			.addressModeV = addressMode,
			.addressModeW = addressMode,
			.mipLodBias = 0.0f,
			.anisotropyEnable = VK_FALSE,
			.maxAnisotropy = 1,
			.compareEnable = VK_FALSE,
			.compareOp = VK_COMPARE_OP_ALWAYS,
			.minLod = 0.0f,
			.maxLod = 0.0f,
			.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
			.unnormalizedCoordinates = VK_FALSE
		};

		VkSampler sampler;

		VkResult res = vkCreateSampler(m_device, &samplerCreateInfo, nullptr, &sampler);
		CHECK_VK_RESULT(res, "vkCreateSampler error\n");

		return sampler;
	}

	void VulkanCore::CreateDepthImages()
	{
		int colorImages = (int)m_Images.size();
		m_depthImages.resize(colorImages);

		VkFormat depthFormat = m_VulkanPhysicalDevices.SelectedDevice().m_depthFormat;

		for (int i = 0; i < m_depthImages.size(); i++)
		{
			VkImageUsageFlags usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			VkMemoryPropertyFlags props = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			CreateImage(m_depthImages[i], m_windowWidth, m_windowHeight, depthFormat, usage, props);

			VkImageLayout oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			VkImageLayout newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			TransitionImageLayout(m_depthImages[i].m_image, depthFormat, oldLayout, newLayout, 1);

			VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
			m_depthImages[i].m_imageView = CreateImageView(m_device, m_depthImages[i].m_image, depthFormat, aspectFlags);
		}
	}
}