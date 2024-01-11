#pragma once

#include <vector>
#include "vulkan/vulkan.h"

namespace comet
{
    class PhysicalDevice
    {
    public:
        PhysicalDevice(VkPhysicalDevice physicalDevice);

        ~PhysicalDevice() = default;

        const VkPhysicalDeviceFeatures& get_features() const;

        const VkPhysicalDeviceProperties& get_properties() const;

        const VkPhysicalDeviceMemoryProperties& get_memory_properties() const;

        const std::vector<VkQueueFamilyProperties>& get_queue_family_properties() const;

    private:
        VkPhysicalDevice m_physicalDevice{VK_NULL_HANDLE};

        // The features that this GPU supports
        VkPhysicalDeviceFeatures m_features{};

        // The features that will be requested to be enabled in the logical device
        VkPhysicalDeviceFeatures m_requested_features{};

        // The GPU properties
        VkPhysicalDeviceProperties m_properties;

        // The GPU memory properties
        VkPhysicalDeviceMemoryProperties m_memory_properties;

        // The GPU queue family properties
        std::vector<VkQueueFamilyProperties> m_queue_family_properties;

    }; // class PhysicalDevice

} // namespace comet