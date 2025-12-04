#pragma once

#include <vector>
#include "SckVK_Utils.h"

namespace sckVK
{
	struct PhysicalDevice
	{
		VkPhysicalDevice m_physicalDevice;
		VkPhysicalDeviceProperties m_physicalDeviceProps;
		std::vector<VkQueueFamilyProperties> m_queueFamilyProps;
		std::vector<VkBool32> m_queueSupportsPresent;
		std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
		VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
		VkPhysicalDeviceMemoryProperties m_memoryProps;
		std::vector<VkPresentModeKHR> m_presentModes;
	};

	class VulkanPhysicalDevices
	{
	public:
		VulkanPhysicalDevices();
		~VulkanPhysicalDevices();

		void Init(const VkInstance& instance, const VkSurfaceKHR& surface);

		uint32_t SelectDevice(VkQueueFlagBits requiredQueueType, bool supportsPresentation);

		const PhysicalDevice& SelectedDevice() const;

	private:
		std::vector<PhysicalDevice> m_physicalDevices;
		int m_deviceIndex = -1;
	};
}