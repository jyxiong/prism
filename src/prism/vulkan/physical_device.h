#pragma once

#include <cstdint>

#include "prism/vulkan/surface.h"

namespace prism
{
  class PhysicalDevice
  {
  public:
    PhysicalDevice(VkPhysicalDevice physical_device);

    PhysicalDevice(const PhysicalDevice &) = delete;

    PhysicalDevice(PhysicalDevice &&) = delete;

    ~PhysicalDevice() = default;

    PhysicalDevice &operator=(const PhysicalDevice &) = delete;

    PhysicalDevice &operator=(PhysicalDevice &&) = delete;

    VkPhysicalDevice get_handle() const;

    const VkPhysicalDeviceFeatures &get_features() const;

    template<typename FeatureStruct, VkStructureType type>
    FeatureStruct getFeatures() const
    {
      FeatureStruct features = {};
      features.sType = type;

      if (get_physical_device_features2)
      {
        VkPhysicalDeviceFeatures2 features2 = {};
        features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features2.pNext = &features;

        get_physical_device_features2(m_handle, &features2);
      }

      return features;
    }

    const VkPhysicalDeviceProperties &get_properties() const;

    const VkPhysicalDeviceMemoryProperties &get_memory_properties() const;

    const std::vector<VkQueueFamilyProperties> &get_queue_family_properties() const;

    const std::vector<VkExtensionProperties> &get_extensions() const;

    VkFormatProperties get_format_properties(VkFormat format) const;

    uint32_t get_queue_family_index(VkQueueFlags flags) const;

    uint32_t get_queue_family_index(VkQueueFlags flags, const Surface &surface) const;

    bool check_extension_support(const std::vector<const char *> &extension_names) const;

  public:
    PFN_vkGetPhysicalDeviceFeatures2 get_physical_device_features2 = nullptr;
    PFN_vkGetPhysicalDeviceProperties2 get_physical_device_properties2 = nullptr;

  private:
    VkPhysicalDevice m_handle{VK_NULL_HANDLE};
    VkPhysicalDeviceFeatures m_features{};
    // VkPhysicalDeviceFeatures m_requested_features{};
    VkPhysicalDeviceProperties m_properties;
    VkPhysicalDeviceMemoryProperties m_memory_properties;
    std::vector<VkQueueFamilyProperties> m_queue_family_properties;
    std::vector<VkExtensionProperties> m_extensions;
  }; // class PhysicalDevice

} // namespace prism