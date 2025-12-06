#include "VulkanApp.h"

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
}

void VulkanApp::RenderScene()
{
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
