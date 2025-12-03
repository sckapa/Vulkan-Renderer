#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>

#define	CHECK_VK_RESULT(res, msg) \
	if(res != VK_SUCCESS) \
	{ \
		fprintf(stderr, "Error in %s:%d - %s, code %x\n", __FILE__, __LINE__, msg, res); \
		exit(1); \
	}

const char* MessageSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT severity);
const char* MessageType(VkDebugUtilsMessageTypeFlagsEXT type);