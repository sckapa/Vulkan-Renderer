#pragma once

#include "SckVK_Core.h"
#include "SckVK_Wrapper.h"
#include "SckVK_Shader.h"
#include "SckVK_VulkanGraphicsPipeline.h"
#include "SckVK_SimpleMesh.h"
#include "SckVK_BufferAndMemory.h"
#include "SckVK_GLFWCallbacks.h"
#include "SckVK_Utils.h"
#include "Camera.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace sckVK
{
	class VulkanApp : public sckVK::GLFWCallbacks
	{
	public:
		VulkanApp(int width, int height);
		~VulkanApp();

		void Init(const char* appName);
		void Execute();
		void RenderScene();

	private:
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecordCommandBuffers();
		void CreateShaders();
		void CreatePipeline();
		void CreateMesh();
		void CreateVertexBuffer();
		void LoadTexture();
		void CreateUniformBuffers();
		void UpdateUniformBuffer(uint32_t imageIndex);

		void CreateCamera();
		virtual void Key(GLFWwindow* window, int key, int scanCode, int action, int mods) override;
		virtual void MouseMove(double xPos, double yPos) override;
		virtual void MouseButton(int button, int action, int mods) override;

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
		std::vector<sckVK::BufferAndMemory> m_uniformBuffers;

		Camera* m_camera = nullptr;
		int m_windowWidth = 0;
		int m_windowHeight = 0;
	};
}