#include "prism/vulkan/physical_device.h"

#include <algorithm>
#include <cstdint>
#include <cstring>

using namespace prism;

PhysicalDevice::PhysicalDevice(VkPhysicalDevice physical_device)
    : m_handle(physical_device)
{
  get_physical_device_features2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2>(vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkGetPhysicalDeviceFeatures2"));
  get_physical_device_properties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkGetPhysicalDeviceProperties2"));

  vkGetPhysicalDeviceFeatures(m_handle, &m_features);
  vkGetPhysicalDeviceProperties(m_handle, &m_properties);
  vkGetPhysicalDeviceMemoryProperties(m_handle, &m_memory_properties);  

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(m_handle, &queue_family_count, nullptr);
  m_queue_family_properties.resize(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(m_handle, &queue_family_count, m_queue_family_properties.data());

  uint32_t extension_count = 0;
  vkEnumerateDeviceExtensionProperties(m_handle, nullptr, &extension_count, nullptr);
  m_extensions.resize(extension_count);
  vkEnumerateDeviceExtensionProperties(m_handle, nullptr, &extension_count, m_extensions.data());
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

const std::vector<VkExtensionProperties> &PhysicalDevice::get_extensions() const
{
  return m_extensions;
}

VkFormatProperties PhysicalDevice::get_format_properties(VkFormat format) const
{
  VkFormatProperties format_properties;
  vkGetPhysicalDeviceFormatProperties(m_handle, format, &format_properties);
  return format_properties;
}

uint32_t PhysicalDevice::get_queue_family_index(VkQueueFlags flags) const
{
 // TODO: check vkGetPhysicalDeviceSurfaceSupportKHR for graphics queue when surface is present
  uint32_t queue_family_index = m_queue_family_properties.size();
  for (uint32_t i = 0; i < m_queue_family_properties.size(); i++)
  {
    const auto& queue_family_properties = m_queue_family_properties[i];
    if ((queue_family_properties.queueFlags & flags) == flags)
    {
      if (queue_family_properties.queueFlags == flags)
      {
        return i;
      }

      if (queue_family_index == m_queue_family_properties.size())
      {
        queue_family_index = i;
      }
    }
  }

  if (queue_family_index < m_queue_family_properties.size())
  {
    return queue_family_index;
  }

  throw std::runtime_error("Could not find a matching queue family index");
}

uint32_t PhysicalDevice::get_queue_family_index(VkQueueFlags flags, const Surface &surface) const
{
  uint32_t queue_family_index = m_queue_family_properties.size();
  for (uint32_t i = 0; i < m_queue_family_properties.size(); i++)
  {
    const auto& queue_family_properties = m_queue_family_properties[i];
    if ((queue_family_properties.queueFlags & flags) == flags)
    {
      VkBool32 present_support = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(m_handle, i, surface.get_handle(), &present_support);
      if (present_support)
      {
        if (queue_family_properties.queueFlags == flags)
        {
          return i;
        }

        if (queue_family_index == m_queue_family_properties.size())
        {
          queue_family_index = i;
        }
      }
    }
  }

  if (queue_family_index < m_queue_family_properties.size())
  {
    return queue_family_index;
  }

  throw std::runtime_error("Could not find a matching queue family index");
}

bool PhysicalDevice::check_extension_support(const std::vector<const char *> &extension_names) const
{
  return std::all_of(extension_names.begin(), extension_names.end(), [&](const char* required_extension)
                     { return std::any_of(m_extensions.begin(), m_extensions.end(), [&](const VkExtensionProperties &extension)
                                         { return std::strcmp(extension.extensionName, required_extension) == 0; }); });
}
