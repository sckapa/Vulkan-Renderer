#pragma once

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "SckVK_VulkanPhysicalDevices.h"
#include "SckVK_VulkanQueue.h"

namespace sckVK
{
	class VulkanCore
	{
	public :
		VulkanCore();
		~VulkanCore();

		void Init(const char* appName, GLFWwindow* window);

		uint32_t GetSwapchainImageCount();
		VkImage& GetImage(uint32_t imgNumber);

		VulkanQueue* GetQueue();

		VkDevice GetDevice();

		void CreateCommandBuffers(uint32_t count, VkCommandBuffer* cmdBuffers);
		void FreeCommandBuffers(uint32_t count, VkCommandBuffer* cmdBuffers);

		VkRenderPass CreateRenderPass();

		std::vector<VkFramebuffer> CreateFrameBuffers(VkRenderPass renderPass);
		void DestroyFrameBuffers();

	private:
		void CreateInstance(const char* appName);
		void CreateDebugCallback();
		void CreateSurface(GLFWwindow* window);
		void CreateDevice();
		void CreateSwapchain();
		void CreateCommandBufferPool();

		uint32_t m_swapchainImageCount = 0;

		VkInstance m_VkInstance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_VkDebugMessenger = VK_NULL_HANDLE;
		VkSurfaceKHR m_VkSurface = VK_NULL_HANDLE;
		VulkanPhysicalDevices m_VulkanPhysicalDevices;
		uint32_t m_queueFamily = 0;
		VkDevice m_device = VK_NULL_HANDLE;
		VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
		std::vector<VkImage> m_Images;
		std::vector<VkImageView> m_ImageViews;
		VkCommandPool m_VkCommandPool = VK_NULL_HANDLE;
		sckVK::VulkanQueue m_vkQueue;
		VkSurfaceFormatKHR m_swapchainSurfaceFormat;
		std::vector<VkFramebuffer> m_frameBuffers;
	};
}