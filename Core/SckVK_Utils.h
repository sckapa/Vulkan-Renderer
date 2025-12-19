#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>
#include <vulkan/vulkan.h>

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