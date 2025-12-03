#pragma once

#include "SckVK_Utils.h"
#include <GLFW/glfw3.h>

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

		VkInstance m_VkInstance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_VkDebugMessenger = VK_NULL_HANDLE;
		VkSurfaceKHR m_VkSurface = VK_NULL_HANDLE;
	};
}