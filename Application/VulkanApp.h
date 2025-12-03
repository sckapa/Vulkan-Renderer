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
	sckVK::VulkanCore m_vkCore;
};

