#pragma once

#include "SckVK_Utils.h"

namespace sckVK
{
	class VulkanCore
	{
	public :
		VulkanCore();
		~VulkanCore();

		void Init(const char* appName);

	private:
		void CreateInstance(const char* appName);

		VkInstance m_VkInstance = nullptr;
	};
}