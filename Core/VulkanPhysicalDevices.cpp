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

		for (uint32_t i = 0; i < numPhysicalDevices; i++)
		{
			VkPhysicalDevice PhysicalDevice = PhyDevices[i];
			m_physicalDevices[i].m_physicalDevice = PhysicalDevice;

			vkGetPhysicalDeviceProperties(PhysicalDevice, &m_physicalDevices[i].m_physicalDeviceProps);

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
}