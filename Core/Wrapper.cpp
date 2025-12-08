#include "SckVK_Wrapper.h"

namespace sckVK
{
	void BeginCommandBuffer(VkCommandBuffer cmdBuffer, VkCommandBufferUsageFlags usageFlags)
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = usageFlags,
			.pInheritanceInfo = nullptr
		};

		VkResult res = vkBeginCommandBuffer(cmdBuffer, &commandBufferBeginInfo);
		CHECK_VK_RESULT(res, "vkBeginCommandBuffer error\n");
	}

	VkSemaphore CreateSemaphore(VkDevice device)
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0
		};

		VkSemaphore semaphore;
		VkResult res = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphore);
		CHECK_VK_RESULT(res, "vkCreateSemaphore error\n");
		return semaphore;
	}
}