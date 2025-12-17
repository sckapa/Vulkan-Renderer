#pragma once

#include "GLFW/glfw3.h"
#include "SckVK_Wrapper.h"

namespace sckVK
{
	class VulkanGraphicsPipeline
	{
	public :
		VulkanGraphicsPipeline(VkDevice device, VkRenderPass renderPass, GLFWwindow* window, VkShaderModule fragmentShader, VkShaderModule vertexShader);
		~VulkanGraphicsPipeline();

		void Bind(VkCommandBuffer buffer);

	private:
		VkDevice m_device;
		VkPipeline m_pipeline;
		VkPipelineLayout m_pipelineLayout;
	};
}