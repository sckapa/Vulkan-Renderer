#pragma once

#include "GLFW/glfw3.h"
#include "SckVK_Wrapper.h"
#include "SckVK_SimpleMesh.h"

namespace sckVK
{
	class VulkanGraphicsPipeline
	{
	public :
		VulkanGraphicsPipeline(VkDevice device, VkRenderPass renderPass, GLFWwindow* window, VkShaderModule fragmentShader, VkShaderModule vertexShader, 
								const SimpleMesh* simpleMesh, uint32_t numImages, std::vector<BufferAndMemory>& uniformBuffers, size_t uniformBufSize);
		~VulkanGraphicsPipeline();

		void Bind(VkCommandBuffer buffer, uint32_t imageIndex);

	private:
		void CreateDescriptorSets(const SimpleMesh* simpleMesh, uint32_t numImages, std::vector<BufferAndMemory> uniformBuffers, size_t uniformBufSize);
		void CreateDescriptorPool(uint32_t numImages);
		void CreateDescriptorLayouts();
		void AllocateDescriptorSets(uint32_t numImages);
		void UpdateDescriptorSets(const SimpleMesh* simpleMesh, uint32_t numImages, std::vector<BufferAndMemory> uniformBuffers, size_t uniformBufSize);

		VkDevice m_device;
		VkPipeline m_pipeline;
		VkPipelineLayout m_pipelineLayout;
		VkDescriptorPool m_descriptorPool;
		VkDescriptorSetLayout m_descriptorSetLayout;
		std::vector<VkDescriptorSet> m_descriptorSets;
	};
}