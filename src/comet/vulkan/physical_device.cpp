#include "comet/vulkan/physical_device.h"

using namespace comet;

PhysicalDevice::PhysicalDevice(VkPhysicalDevice physicalDevice)
    : m_handle(physicalDevice)
{
    // Get the features of the GPU
    vkGetPhysicalDeviceFeatures(m_handle, &m_features);

    // Get the properties of the GPU
    vkGetPhysicalDeviceProperties(m_handle, &m_properties);

    // Get the memory properties of the GPU
    vkGetPhysicalDeviceMemoryProperties(m_handle, &m_memory_properties);

    // Get the queue family properties of the GPU
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_handle, &queue_family_count, nullptr);
    m_queue_family_properties.resize(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(m_handle, &queue_family_count, m_queue_family_properties.data());
}

VkPhysicalDevice PhysicalDevice::get_handle() const
{
    return m_handle;
}

const VkPhysicalDeviceFeatures &PhysicalDevice::get_features() const
{
    return m_features;
}

const VkPhysicalDeviceProperties &PhysicalDevice::get_properties() const
{
    return m_properties;
}

const VkPhysicalDeviceMemoryProperties &PhysicalDevice::get_memory_properties() const
{
    return m_memory_properties;
}

const std::vector<VkQueueFamilyProperties> &PhysicalDevice::get_queue_family_properties() const
{
    return m_queue_family_properties;
}

VkBool32 PhysicalDevice::is_present_supported(VkSurfaceKHR surface, uint32_t queue_family_index) const
{
    VkBool32 present_supported = VK_FALSE;
    if (surface != VK_NULL_HANDLE)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR(m_handle, queue_family_index, surface, &present_supported);
    }
    return present_supported;
}
