#pragma once

#include "volk.h"

namespace prism
{
  class PhysicalDevice
  {
  public:
    PhysicalDevice(VkPhysicalDevice physical_device);

    ~PhysicalDevice() = default;

    VkPhysicalDevice get_handle() const;

    const VkPhysicalDeviceFeatures &get_features() const;

    const VkPhysicalDeviceProperties &get_properties() const;

    const VkPhysicalDeviceMemoryProperties &get_memory_properties() const;

    const std::vector<VkQueueFamilyProperties> &get_queue_family_properties() const;

    VkFormatProperties get_format_properties(VkFormat format) const;

    uint32_t get_queue_family_index(VkQueueFlags flags) const;

    bool check_extension(const std::string& extension) const;

  private:
    VkPhysicalDevice m_handle{VK_NULL_HANDLE};
    VkPhysicalDeviceFeatures m_features{};
    VkPhysicalDeviceFeatures m_requested_features{};
    VkPhysicalDeviceProperties m_properties;
    VkPhysicalDeviceMemoryProperties m_memory_properties;
    std::vector<VkQueueFamilyProperties> m_queue_family_properties;
    std::vector<VkExtensionProperties> m_extensions;
  }; // class PhysicalDevice

} // namespace prism