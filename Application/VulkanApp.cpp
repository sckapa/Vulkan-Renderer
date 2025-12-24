#include "VulkanApp.h"

namespace sckVK
{
	struct UniformData
	{
		glm::mat4 WVP;
	};

	VulkanApp::VulkanApp()
	{
	}

	VulkanApp::~VulkanApp()
	{
		printf("\n");

		FreeCommandBuffers();
		printf("Command Buffers Destroyed\n");

		m_vkCore.DestroyFrameBuffers();
		printf("Frame Buffers Destroyed\n");

		vkDestroyShaderModule(m_vkCore.GetDevice(), m_fragmentShader, nullptr);
		printf("Fragment Shader Destroyed\n");

		vkDestroyShaderModule(m_vkCore.GetDevice(), m_vertexShader, nullptr);
		printf("Vertex Shader Destroyed\n");

		delete m_graphicsPipeline;

		vkDestroyRenderPass(m_vkCore.GetDevice(), m_renderPass, nullptr);
		printf("Render Pass Destroyed\n");

		m_simpleMesh.Destroy(m_vkCore.GetDevice());
	}

	void VulkanApp::Init(const char* appName, GLFWwindow* window)
	{
		m_window = window;

		m_vkCore.Init(appName, window);
		m_imageCount = m_vkCore.GetSwapchainImageCount();
		m_vulkanQueue = m_vkCore.GetQueue();
		m_renderPass = m_vkCore.CreateRenderPass();
		m_frameBuffers = m_vkCore.CreateFrameBuffers(m_renderPass);
		CreateShaders();
		CreateVertexBuffer();
		CreateUniformBuffers();
		CreatePipeline();
		CreateCommandBuffers();
		RecordCommandBuffers();
	}

	void VulkanApp::RenderScene()
	{
		uint32_t imgIndex = m_vulkanQueue->AcquireNextImage();

		UpdateUniformBuffer(imgIndex);

		m_vulkanQueue->SubmitAsync(m_commandBuffers[imgIndex]);

		m_vulkanQueue->WaitIdle();

		m_vulkanQueue->Present(imgIndex);
	}

	void VulkanApp::CreateCommandBuffers()
	{
		m_commandBuffers.resize(m_imageCount);
		m_vkCore.CreateCommandBuffers(m_imageCount, m_commandBuffers.data());
	}

	void VulkanApp::FreeCommandBuffers()
	{
		m_vkCore.FreeCommandBuffers(m_imageCount, m_commandBuffers.data());
	}

	void VulkanApp::RecordCommandBuffers()
	{
		VkClearColorValue clearColorValue = { 1.0f, 0.0f, 0.0f, 0.0f };

		VkClearValue clearValue = {
			.color = clearColorValue
		};

		VkRenderPassBeginInfo renderPassBeginInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.pNext = nullptr,
			.renderPass = m_renderPass,
			.renderArea = {
				.offset =
				{
					.x = 0,
					.y = 0
				},
				.extent =
				{
				.width = WINDOW_WIDTH,
				.height = WINDOW_HEIGHT
				}
			},
			.clearValueCount = 1,
			.pClearValues = &clearValue
		};

		for (uint32_t i = 0; i < m_commandBuffers.size(); i++)
		{
			sckVK::BeginCommandBuffer(m_commandBuffers[i], VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

			renderPassBeginInfo.framebuffer = m_frameBuffers[i];

			vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			m_graphicsPipeline->Bind(m_commandBuffers[i], i);

			uint32_t vertexCount = 3;
			uint32_t instanceCount = 1;
			uint32_t firstVertex = 0;
			uint32_t firstInstance = 0;

			vkCmdDraw(m_commandBuffers[i], vertexCount, instanceCount, firstVertex, firstInstance);

			vkCmdEndRenderPass(m_commandBuffers[i]);

			VkResult res = vkEndCommandBuffer(m_commandBuffers[i]);
			CHECK_VK_RESULT(res, "vkEndCommandBuffer error\n");
		}

		printf("Clear command recorded\n");
	}

	void VulkanApp::CreateShaders()
	{
		m_vertexShader = sckVK::CreateShaderModuleFromText(m_vkCore.GetDevice(), "test.vert");
		printf("Vertex Shader Created\n");
		m_fragmentShader = sckVK::CreateShaderModuleFromText(m_vkCore.GetDevice(), "test.frag");
		printf("Fragment Shader Created\n");
	}

	void VulkanApp::CreatePipeline()
	{
		m_graphicsPipeline = new sckVK::VulkanGraphicsPipeline(m_vkCore.GetDevice(), m_renderPass, m_window, m_fragmentShader, m_vertexShader, &m_simpleMesh, m_imageCount, m_uniformBuffers, sizeof(UniformData));
	}

	void VulkanApp::CreateVertexBuffer()
	{
		struct Vertex
		{
		public:
			Vertex(const glm::vec3& pos, const glm::vec2& tex)
			{
				position = pos;
				texCoord = tex;
			}

			glm::vec3 position;
			glm::vec2 texCoord;
		};

		std::vector<Vertex> vertices =
		{
			Vertex({-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}),
			Vertex({1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}),
			Vertex({0.0f, 1.0f, 0.0f}, {1.0f, 1.0f})
		};

		m_simpleMesh.m_vertexBufferSize = sizeof(vertices[0]) * vertices.size();
		m_simpleMesh.m_vertexBuffer = m_vkCore.CreateVertexBuffer(vertices.data(), m_simpleMesh.m_vertexBufferSize);
	}

	void VulkanApp::CreateUniformBuffers()
	{
		m_uniformBuffers = m_vkCore.CreateUniformBuffers(sizeof(UniformData));
	}

	void VulkanApp::UpdateUniformBuffer(uint32_t imageIndex)
	{
		static float rotAngle = 0.0f;
		glm::mat4 rotate = glm::mat4(1.0f);
		glm::mat4 rotation = glm::rotate(rotate, glm::radians(rotAngle), glm::normalize(glm::vec3(0.0f, 0.0, 1.0f)));
		rotAngle += 0.001f;

		glm::mat4 WVP = rotation;
		m_uniformBuffers[imageIndex].Update(m_vkCore.GetDevice(), &WVP, sizeof(WVP));
	}
}