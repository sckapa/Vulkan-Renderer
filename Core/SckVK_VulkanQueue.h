#pragma once

#include "SckVK_Wrapper.h"

namespace sckVK
{
	class VulkanQueue
	{
	public :
		VulkanQueue();
		~VulkanQueue();

		void Init(VkDevice device, VkSwapchainKHR swapchain, uint32_t queueFamilyIndex, uint32_t queueIndex);

		void DestroySemaphores();

		uint32_t AcquireNextImage();

		void SubmitAsync(VkCommandBuffer cmdBuffers);
		void SubmitSync(VkCommandBuffer cmdBuffers);

		void Present(uint32_t imageIndex);

		void WaitIdle();

	private:
		void CreateSemaphores();

		VkQueue m_queue = VK_NULL_HANDLE;
		VkDevice m_device = VK_NULL_HANDLE;
		VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
		VkSemaphore m_renderCompleteSemaphore;
		VkSemaphore m_presentCompleteSemaphore;
	};
}