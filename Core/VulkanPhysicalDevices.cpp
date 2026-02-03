#include "SckVK_VulkanPhysicalDevices.h"

namespace sckVK
{
	VulkanPhysicalDevices::VulkanPhysicalDevices()
	{
	}

	VulkanPhysicalDevices::~VulkanPhysicalDevices()
	{
	}

	void VulkanPhysicalDevices::Init(const VkInstance& instance, const VkSurfaceKHR& surface)
	{
		uint32_t numPhysicalDevices;
		VkResult res = vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, nullptr);
		CHECK_VK_RESULT(res, "vkEnumeratePhysicalDevices error\n");

		std::vector<VkPhysicalDevice> PhyDevices;
		PhyDevices.resize(numPhysicalDevices);
		m_physicalDevices.resize(numPhysicalDevices);

		res = vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, PhyDevices.data());
		CHECK_VK_RESULT(res, "vkEnumeratePhysicalDevices error\n");

		printf("Available graphics devices : \n");
		for (uint32_t i = 0; i < numPhysicalDevices; i++)
		{
			VkPhysicalDevice PhysicalDevice = PhyDevices[i];
			m_physicalDevices[i].m_physicalDevice = PhysicalDevice;

			vkGetPhysicalDeviceProperties(PhysicalDevice, &m_physicalDevices[i].m_physicalDeviceProps);
			printf("  %s\n", m_physicalDevices[i].m_physicalDeviceProps.deviceName);

			uint32_t numQueueProperties;
			vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &numQueueProperties, nullptr);

			m_physicalDevices[i].m_queueFamilyProps.resize(numQueueProperties);
			m_physicalDevices[i].m_queueSupportsPresent.resize(numQueueProperties);

			vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &numQueueProperties, m_physicalDevices[i].m_queueFamilyProps.data());

			for (uint32_t q = 0; q < numQueueProperties; q++)
			{
				res = vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, q, surface, &(m_physicalDevices[i].m_queueSupportsPresent[q]));
				CHECK_VK_RESULT(res, "vkGetPhysicalDeviceSurfaceSupportKHR error\n");
			}


			res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, surface, &(m_physicalDevices[i].m_surfaceCapabilities));
			CHECK_VK_RESULT(res, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR error\n");

			vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &(m_physicalDevices[i].m_memoryProps));

			uint32_t numSurfacePresentModes;
			res = vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, surface, &numSurfacePresentModes, nullptr);
			CHECK_VK_RESULT(res, "vkGetPhysicalDeviceSurfacePresentModesKHR error\n");

			m_physicalDevices[i].m_presentModes.resize(numSurfacePresentModes);

			res = vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, surface, &numSurfacePresentModes, m_physicalDevices[i].m_presentModes.data());
			CHECK_VK_RESULT(res, "vkGetPhysicalDeviceSurfacePresentModesKHR error\n");

			vkGetPhysicalDeviceFeatures(PhysicalDevice, &m_physicalDevices[i].m_physicalDeviceFeatures);

			uint32_t surfaceFormatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, surface, &surfaceFormatCount, nullptr);

			m_physicalDevices[i].m_surfaceFormats.resize(surfaceFormatCount);

			vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, surface, &surfaceFormatCount, m_physicalDevices[i].m_surfaceFormats.data());

			m_physicalDevices[i].m_depthFormat = GetDepthFormat(PhysicalDevice);
		}
	}

	uint32_t VulkanPhysicalDevices::SelectDevice(VkQueueFlagBits requiredQueueType, bool supportsPresentation)
	{
		for (uint32_t i = 0; i < m_physicalDevices.size(); i++)
		{
			for (uint32_t j = 0; j < m_physicalDevices[i].m_queueFamilyProps.size(); j++)
			{
				if ((m_physicalDevices[i].m_queueFamilyProps[j].queueFlags & requiredQueueType) && ((bool)m_physicalDevices[i].m_queueSupportsPresent[j] == supportsPresentation))
				{
					m_deviceIndex = j;
					return m_deviceIndex;
				}
			}
		}

		printf("Invalid VkQueueFlagBits\n");
		exit(0);
	}

	const PhysicalDevice& VulkanPhysicalDevices::SelectedDevice() const
	{
		if (m_deviceIndex < 0)
		{
			printf("Physical device has not been selected\n");
			exit(0);
		}

		return m_physicalDevices[m_deviceIndex];
	}

	const VkFormat VulkanPhysicalDevices::GetDepthFormat(VkPhysicalDevice device) const
	{
		std::vector<VkFormat> candidates =
		{
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT
		};

		VkFormat depthFormat = FindSupportedFormat(device, candidates, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

		return depthFormat;
	}

	const VkFormat VulkanPhysicalDevices::FindSupportedFormat(VkPhysicalDevice device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
	{
		for (int i = 0; i < candidates.size(); i++)
		{
			VkFormat format = candidates[i];
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(device, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR)
			{
				if ((props.linearTilingFeatures & features) == features)
				{
					return format;
				}
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL)
			{
				if ((props.optimalTilingFeatures & features) == features)
				{
					return format;
				}
			}
		}
	}
}