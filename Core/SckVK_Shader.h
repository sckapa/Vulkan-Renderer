#pragma once

#include "SckVK_Wrapper.h"

namespace sckVK
{
	VkShaderModule CreateShaderModuleFromText(VkDevice device, const char* fileName);
	VkShaderModule CreateShaderModuleFromBinary(VkDevice device, const char* fileName);
}