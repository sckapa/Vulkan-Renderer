#include <stdlib.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanApp.h"
#include <iostream>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define APP_NAME "Vulkan Renderer"

GLFWwindow* window = nullptr;

void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS))
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

int main(int argc, char* argv[])
{
	if (!glfwInit())
	{
		return 1;
	}

	if (!glfwVulkanSupported())
	{
		return 1;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APP_NAME, nullptr, nullptr);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, GLFW_KeyCallback);

	VulkanApp App;
	App.Init(APP_NAME);

	while (!glfwWindowShouldClose(window))
	{
		App.RenderScene();
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}