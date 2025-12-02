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
	}

	void VulkanCore::Init(const char* appName)
	{
		CreateInstance(appName);
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
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME
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
}