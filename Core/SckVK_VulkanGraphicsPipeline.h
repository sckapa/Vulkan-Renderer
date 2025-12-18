#pragma once

#include "GLFW/glfw3.h"
#include "SckVK_Wrapper.h"
#include "SckVK_SimpleMesh.h"

namespace sckVK
{
	class VulkanGraphicsPipeline
	{
	public :
		VulkanGraphicsPipeline(VkDevice device, VkRenderPass renderPass, GLFWwindow* window, VkShaderModule fragmentShader, VkShaderModule vertexShader, SimpleMesh* simpleMesh = nullptr);
		~VulkanGraphicsPipeline();

		void Bind(VkCommandBuffer buffer);

	private:
		VkDevice m_device;
		VkPipeline m_pipeline;
		VkPipelineLayout m_pipelineLayout;
	};
}