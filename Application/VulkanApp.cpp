#include "VulkanApp.h"

VulkanApp::VulkanApp()
{
}

VulkanApp::~VulkanApp()
{
}

void VulkanApp::Init(const char* appName, GLFWwindow* window)
{
	m_vkCore.Init(appName, window);
}

void VulkanApp::RenderScene()
{
}
