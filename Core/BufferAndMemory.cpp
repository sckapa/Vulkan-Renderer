#include "SckVK_BufferAndMemory.h"

namespace sckVK
{
	void BufferAndMemory::Destroy(VkDevice device)
	{
		vkDestroyBuffer(device, m_buffer, nullptr);
		vkFreeMemory(device, m_memory, nullptr);
	}
}