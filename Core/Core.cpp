#include "SckVK_Core.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace sckVK
{
	VulkanCore::VulkanCore()
	{
	}

	VulkanCore::~VulkanCore()
	{
		printf("\n");

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
		printf("Debug Callback : %s", pCallbackData->pMessage);
		printf("Severity : %s", MessageSeverity(messageSeverity));
		printf("Type : %s", MessageType(messageTypes));
		printf(" Objects ");

		for (uint32_t i = 0; i < pCallbackData->objectCount; i++)
		{
			printf("%llx", pCallbackData->pObjects[i].objectHandle);
		}

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
}