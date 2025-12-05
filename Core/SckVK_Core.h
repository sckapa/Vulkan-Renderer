#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "SckVK_VulkanPhysicalDevices.h"

namespace sckVK
{
	class VulkanCore
	{
	public :
		VulkanCore();
		~VulkanCore();

		void Init(const char* appName, GLFWwindow* window);

	private:
		void CreateInstance(const char* appName);
		void CreateDebugCallback();
		void CreateSurface(GLFWwindow* window);
		void CreateDevice();

		VkInstance m_VkInstance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_VkDebugMessenger = VK_NULL_HANDLE;
		VkSurfaceKHR m_VkSurface = VK_NULL_HANDLE;
		VulkanPhysicalDevices m_VulkanPhysicalDevices;
		uint32_t m_queueFamily = 0;
		VkDevice m_device = VK_NULL_HANDLE;
	};
}