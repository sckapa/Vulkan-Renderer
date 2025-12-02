#pragma once

#include "SckVK_Core.h"

class VulkanApp
{
public:

	VulkanApp();
	~VulkanApp();

	void Init(const char* appName);
	void RenderScene();

private:
	sckVK::VulkanCore m_vkCore;
};

