#pragma once

#include "SckVK_Wrapper.h"
#include "SckVK_BufferAndMemory.h"

namespace sckVK
{
	struct SimpleMesh
	{
	public:
		BufferAndMemory m_vertexBuffer;
		size_t m_vertexBufferSize = 0;

		void Destroy(VkDevice device)
		{
			m_vertexBuffer.Destroy(device);
		}
	};
}