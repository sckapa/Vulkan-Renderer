#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "SckVK_GLFWCallbacks.h"

#define	CHECK_VK_RESULT(res, msg) \
	if(res != VK_SUCCESS) \
	{ \
		fprintf(stderr, "Error in %s:%d - %s, code %x\n", __FILE__, __LINE__, msg, res); \
		exit(1); \
	}

const char* MessageSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT severity);
const char* MessageType(VkDebugUtilsMessageTypeFlagsEXT type);
bool ReadFile(const char* fileName, std::string& data);
void WriteBinaryFile(const char* pFilename, const void* pData, int size);
char* ReadBinaryFile(const char* pFilename, int& size);
bool HasStencilComponent(VkFormat Format);
GLFWwindow* InitGLFW_Vulkan(int width, int height, const char* title);
void SetGLFWCallbacks_Vulkan(GLFWwindow * window, sckVK::GLFWCallbacks* callbacks);
void GLFW_KeyCallback(GLFWwindow * window, int key, int scanCode, int action, int mods);
void GLFW_CursorCallback(GLFWwindow * window, double xPos, double yPos);
void GLFW_MouseButtonCallback(GLFWwindow * window, int button, int action, int mods);