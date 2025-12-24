#include "SckVK_BufferAndMemory.h"

namespace sckVK
{
	void BufferAndMemory::Destroy(VkDevice device)
	{
		vkDestroyBuffer(device, m_buffer, nullptr);
		vkFreeMemory(device, m_memory, nullptr);
	}

	void BufferAndMemory::Update(VkDevice device, const void* data, size_t size)
	{
		void* mem = nullptr;
		VkResult res = vkMapMemory(device, m_memory, 0, size, 0, &mem);
		CHECK_VK_RESULT(res, "vkMapMemory error\n");
		memcpy(mem, data, size);
		vkUnmapMemory(device, m_memory);
	}
}