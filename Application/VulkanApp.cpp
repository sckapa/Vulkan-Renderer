#include "VulkanApp.h"
#include "SckVK_Wrapper.h"

VulkanApp::VulkanApp()
{
}

VulkanApp::~VulkanApp()
{
	printf("\n");

	FreeCommandBuffers();
}

void VulkanApp::Init(const char* appName, GLFWwindow* window)
{
	m_vkCore.Init(appName, window);
	m_imageCount = m_vkCore.GetSwapchainImageCount();
	CreateCommandBuffers();
	RecordCommandBuffers();
	m_vulkanQueue = m_vkCore.GetQueue();
}

void VulkanApp::RenderScene()
{
	uint32_t imgIndex = m_vulkanQueue->AcquireNextImage();

	m_vulkanQueue->SubmitAsync(m_commandBuffers[imgIndex]);

	m_vulkanQueue->Present(imgIndex);
}

void VulkanApp::CreateCommandBuffers()
{
	m_commandBuffers.resize(m_imageCount);
	m_vkCore.CreateCommandBuffers(m_imageCount, m_commandBuffers.data());
}

void VulkanApp::FreeCommandBuffers()
{
	m_vkCore.FreeCommandBuffers(m_imageCount, m_commandBuffers.data());
}

void VulkanApp::RecordCommandBuffers()
{
	VkClearColorValue clearColorValue = { 1.0f, 0.0f, 0.0f, 0.0f };

	VkImageSubresourceRange imageSubresourceRange = {
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1
	};

	for (uint32_t i = 0; i < m_commandBuffers.size(); i++)
	{
		sckVK::BeginCommandBuffer(m_commandBuffers[i], VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

		vkCmdClearColorImage(m_commandBuffers[i], m_vkCore.GetImage(i), VK_IMAGE_LAYOUT_GENERAL, &clearColorValue, 1, &imageSubresourceRange);

		VkResult res = vkEndCommandBuffer(m_commandBuffers[i]);
		CHECK_VK_RESULT(res, "vkEndCommandBuffer error\n");
	}

	printf("Clear command recorded\n");
}
