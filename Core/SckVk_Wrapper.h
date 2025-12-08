#pragma once

#include "SckVK_Utils.h"

namespace sckVK 
{
	void BeginCommandBuffer(VkCommandBuffer cmdBuffer, VkCommandBufferUsageFlags usageFlags);

	VkSemaphore CreateSemaphore(VkDevice device);
}