#include "SckVK_VulkanQueue.h"

namespace sckVK
{
	VulkanQueue::VulkanQueue()
	{
	}

	VulkanQueue::~VulkanQueue()
	{
	}

	void VulkanQueue::Init(VkDevice device, VkSwapchainKHR swapchain, uint32_t queueFamilyIndex, uint32_t queueIndex)
	{
		m_device = device;
		m_swapchain = swapchain;

		vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &m_queue);
		printf("Queue acquired\n");

		CreateSemaphores();
	}

	void VulkanQueue::DestroySemaphores()
	{
		vkDestroySemaphore(m_device, m_presentCompleteSemaphore, nullptr);
		vkDestroySemaphore(m_device, m_renderCompleteSemaphore, nullptr);
	}

	void VulkanQueue::CreateSemaphores()
	{
		m_presentCompleteSemaphore = CreateSemaphore(m_device);
		m_renderCompleteSemaphore = CreateSemaphore(m_device);
	}

	uint32_t VulkanQueue::AcquireNextImage()
	{
		uint32_t m_imageIndex = 0;
		VkResult res = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_presentCompleteSemaphore, nullptr, &m_imageIndex);
		CHECK_VK_RESULT(res, "vkAcquireNextImageKHR error\n");
		return m_imageIndex;
	}

	void VulkanQueue::SubmitAsync(VkCommandBuffer cmdBuffers)
	{
		VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo submitInfo = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext = nullptr,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &m_presentCompleteSemaphore,
			.pWaitDstStageMask = &waitDstStageMask,
			.commandBufferCount = 1,
			.pCommandBuffers = &cmdBuffers,
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &m_renderCompleteSemaphore
		};

		VkResult res = vkQueueSubmit(m_queue, 1, &submitInfo, nullptr);
		CHECK_VK_RESULT(res, "vkQueueSubmit error\n");
	}

	void VulkanQueue::SubmitSync(VkCommandBuffer cmdBuffers)
	{
		VkSubmitInfo submitInfo = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext = nullptr,
			.waitSemaphoreCount = 0,
			.pWaitSemaphores = VK_NULL_HANDLE,
			.pWaitDstStageMask = VK_NULL_HANDLE,
			.commandBufferCount = 1,
			.pCommandBuffers = &cmdBuffers,
			.signalSemaphoreCount = 0,
			.pSignalSemaphores = VK_NULL_HANDLE
		};

		VkResult res = vkQueueSubmit(m_queue, 1, &submitInfo, nullptr);
		CHECK_VK_RESULT(res, "vkQueueSubmit error\n");
	}

	void VulkanQueue::Present(uint32_t imageIndex)
	{
		VkPresentInfoKHR presentInfo = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.pNext = nullptr,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &m_renderCompleteSemaphore,
			.swapchainCount = 1,
			.pSwapchains = &m_swapchain,
			.pImageIndices = &imageIndex
		};
		VkResult res = vkQueuePresentKHR(m_queue, &presentInfo);
		CHECK_VK_RESULT(res, "vkQueuePresentKHR error\n");
	}

	void VulkanQueue::WaitIdle()
	{
		vkQueueWaitIdle(m_queue);
	}

}