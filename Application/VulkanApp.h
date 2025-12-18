#pragma once

#include "SckVK_Core.h"
#include "SckVK_Wrapper.h"
#include "SckVK_Shader.h"
#include "SckVK_VulkanGraphicsPipeline.h"
#include "SckVK_SimpleMesh.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class VulkanApp
{
public:
	VulkanApp();
	~VulkanApp();

	void Init(const char* appName, GLFWwindow* window);
	void RenderScene();

private:
	void CreateCommandBuffers();
	void FreeCommandBuffers();
	void RecordCommandBuffers();
	void CreateShaders();
	void CreatePipeline();
	void CreateVertexBuffer();

	GLFWwindow* m_window = nullptr;

	sckVK::VulkanCore m_vkCore;
	uint32_t m_imageCount = 0;
	std::vector<VkCommandBuffer> m_commandBuffers;
	sckVK::VulkanQueue* m_vulkanQueue = nullptr;
	VkRenderPass m_renderPass;
	std::vector<VkFramebuffer> m_frameBuffers;
	VkShaderModule m_vertexShader;
	VkShaderModule m_fragmentShader;
	sckVK::VulkanGraphicsPipeline* m_graphicsPipeline = nullptr;
	sckVK::SimpleMesh m_simpleMesh;
};

