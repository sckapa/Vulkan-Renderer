#include "SckVk_Wrapper.h"

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
}