#include "SckVK_VulkanGraphicsPipeline.h"

namespace sckVK
{
	VulkanGraphicsPipeline::VulkanGraphicsPipeline(VkDevice device, VkRenderPass renderPass, GLFWwindow* window, VkShaderModule fragmentShader, 
													VkShaderModule vertexShader, const SimpleMesh* simpleMesh, uint32_t numImages)
	{
		m_device = device;

		CreateDescriptorSets(simpleMesh, numImages);

		VkPipelineShaderStageCreateInfo shaderStageCreateInfo[2] = {
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.stage = VK_SHADER_STAGE_VERTEX_BIT,
				.module = vertexShader,
				.pName = "main"
			},
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
				.module = fragmentShader,
				.pName = "main"
			}
		};

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
		};

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE
		};

		int width, height;
		glfwGetWindowSize(window, &width, &height);

		VkViewport viewPort = {
			.x = 0.0f,
			.y = 0.0f,
			.width = (float)width,
			.height = (float)height,
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};

		VkRect2D rect2D = {
			.offset =
			{
				.x = 0,
				.y = 0
			},
			.extent =
			{
				.width = (uint32_t)width,
				.height = (uint32_t)height
			}
		};

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.pViewports = &viewPort,
			.scissorCount = 1,
			.pScissors = &rect2D
		};

		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.polygonMode = VK_POLYGON_MODE_FILL,
			.cullMode = VK_CULL_MODE_NONE,
			.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
			.lineWidth = 1.0f
		};

		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable = VK_FALSE,
			.minSampleShading = 1.0f
		};

		VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
			.blendEnable = VK_FALSE,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		};

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY,
			.attachmentCount = 1,
			.pAttachments = &colorBlendAttachmentState
		};

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = 1,
			.pSetLayouts = &m_descriptorSetLayout
		};

		VkResult res = vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);
		CHECK_VK_RESULT(res, "vkCreatePipelineLayout error\n");

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stageCount = 2,
			.pStages = shaderStageCreateInfo,
			.pVertexInputState = &vertexInputStateCreateInfo,
			.pInputAssemblyState = &inputAssemblyStateCreateInfo,
			.pViewportState = &viewportStateCreateInfo,
			.pRasterizationState = &rasterizationStateCreateInfo,
			.pMultisampleState = &multisampleStateCreateInfo,
			.pColorBlendState = &colorBlendStateCreateInfo,
			.layout = m_pipelineLayout,
			.renderPass = renderPass,
			.subpass = 0
		};

		res = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_pipeline);
		CHECK_VK_RESULT(res, "vkCreateGraphicsPipelines error\n");

		printf("Graphics Pipeline Created\n");
	}

	VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
	{
		vkFreeDescriptorSets(m_device, m_descriptorPool, (uint32_t)m_descriptorSets.size(), m_descriptorSets.data());
		vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);

		vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
		vkDestroyPipeline(m_device, m_pipeline, nullptr);
	}

	void VulkanGraphicsPipeline::Bind(VkCommandBuffer buffer, uint32_t imageIndex)
	{
		vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

		vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSets[imageIndex], 0, nullptr);
	}

	void VulkanGraphicsPipeline::CreateDescriptorSets(const SimpleMesh* simpleMesh, uint32_t numImages)
	{
		CreateDescriptorPool(numImages);
		CreateDescriptorLayouts();
		AllocateDescriptorSets(numImages);
		UpdateDescriptorSets(simpleMesh, numImages);
	}

	void VulkanGraphicsPipeline::CreateDescriptorPool(uint32_t numImages)
	{
		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.flags = 0,
			.maxSets = (uint32_t)numImages,
			.poolSizeCount = 0,
			.pPoolSizes = nullptr
		};

		VkResult res = vkCreateDescriptorPool(m_device, &descriptorPoolCreateInfo, nullptr, &m_descriptorPool);
		CHECK_VK_RESULT(res, "vkCreateDescriptorPool error\n");

		printf("Descriptor Pool Created\n");
	}

	void VulkanGraphicsPipeline::CreateDescriptorLayouts()
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT
		};

		bindings.push_back(descriptorSetLayoutBinding);

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.flags = 0,
			.bindingCount = (uint32_t)bindings.size(),
			.pBindings = bindings.data()
		};

		VkResult res = vkCreateDescriptorSetLayout(m_device, &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout);
		CHECK_VK_RESULT(res, "vkCreateDescriptorSetLayout error\n");

		printf("Descriptor Set Layout Created\n");
	}

	void VulkanGraphicsPipeline::AllocateDescriptorSets(uint32_t numImages)
	{
		std::vector<VkDescriptorSetLayout> layouts(numImages, m_descriptorSetLayout);

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = m_descriptorPool,
			.descriptorSetCount = (uint32_t)numImages,
			.pSetLayouts = layouts.data()
		};

		m_descriptorSets.resize(numImages);

		VkResult res = vkAllocateDescriptorSets(m_device, &descriptorSetAllocateInfo, m_descriptorSets.data());
		CHECK_VK_RESULT(res, "vkAllocateDescriptorSets error\n");
	}

	void VulkanGraphicsPipeline::UpdateDescriptorSets(const SimpleMesh* simpleMesh, uint32_t numImages)
	{
		VkDescriptorBufferInfo descriptorBufferInfo = {
			.buffer = simpleMesh->m_vertexBuffer.m_buffer,
			.offset = 0,
			.range = simpleMesh->m_vertexBufferSize
		};

		std::vector<VkWriteDescriptorSet> writeDescriptorSets;

		for (uint32_t i = 0; i < numImages; i++)
		{
			writeDescriptorSets.push_back
			(
				VkWriteDescriptorSet
				{
					.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					.dstSet = m_descriptorSets[i],
					.dstBinding = 0,
					.dstArrayElement = 0,
					.descriptorCount = 1,
					.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
					.pBufferInfo = &descriptorBufferInfo
				}
			);
		}

		vkUpdateDescriptorSets(m_device, (uint32_t)writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
	}
}