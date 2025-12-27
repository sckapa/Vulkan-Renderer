#pragma once

#include "SckVK_Wrapper.h"

namespace sckVK
{
	class BufferAndMemory
	{
	public:
		VkBuffer m_buffer = nullptr;
		VkDeviceMemory m_memory = nullptr;
		VkDeviceSize m_allocationSize = 0;

		void Destroy(VkDevice device);
		void Update(VkDevice device, const void* data, size_t size);
	};

	class VulkanTexture
	{
	public:
		VkImage m_image = nullptr;
		VkDeviceMemory m_memory = nullptr;
		VkImageView m_imageView = nullptr;
		VkSampler m_sampler = nullptr;

		void Destroy(VkDevice device);
	};
}