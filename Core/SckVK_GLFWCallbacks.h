#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace sckVK
{
	class GLFWCallbacks
	{
	public:
		virtual void Key(GLFWwindow* window, int key, int scanCode, int action, int mods) = 0;
		virtual void MouseMove(double xPos, double yPos) = 0;
		virtual void MouseButton(int button, int action, int mods) = 0;
	};
}
