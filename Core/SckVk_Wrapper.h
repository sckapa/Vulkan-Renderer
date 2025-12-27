#pragma once

#include "SckVK_Utils.h"

namespace sckVK 
{
	void BeginCommandBuffer(VkCommandBuffer cmdBuffer, VkCommandBufferUsageFlags usageFlags);

	VkSemaphore CreateSemaphore(VkDevice device);

	void ImageMemBarrier(VkCommandBuffer CmdBuf, VkImage Image, VkFormat Format, VkImageLayout OldLayout, VkImageLayout NewLayout, int LayerCount);

	VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
}