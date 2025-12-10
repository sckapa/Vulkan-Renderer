#pragma once

#include "SckVK_Core.h"
#include <GLFW/glfw3.h>

class VulkanApp
{
public:

	VulkanApp();
	~VulkanApp();

	void Init(const char* appName, GLFWwindow* window);
	void RenderScene();

private:
	void CreateCommandBuffers();
	void FreeCommandBuffers();
	void RecordCommandBuffers();

	sckVK::VulkanCore m_vkCore;
	uint32_t m_imageCount = 0;
	std::vector<VkCommandBuffer> m_commandBuffers;
	sckVK::VulkanQueue* m_vulkanQueue = nullptr;
	VkRenderPass m_renderPass;
	std::vector<VkFramebuffer> m_frameBuffers;
};

