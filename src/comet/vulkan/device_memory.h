#pragma once



#include "comet/vulkan/device.h"

namespace comet
{
  class DeviceMemory
  {
  public:
    DeviceMemory(const Device& device, const VkMemoryRequirements& requirements, VkMemoryPropertyFlags property_flags, VkMemoryAllocateFlags allocate_flags = 0);

    ~DeviceMemory();

    void copy(VkDeviceSize offset, VkDeviceSize size, const void* src_data);

    void map(VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData);

    void unmap();

    VkDeviceMemory get_handle() const;
    const Device &get_device() const;
    VkMemoryRequirements get_requirements() const;
    VkMemoryPropertyFlags get_properties() const;

  private:
    VkDeviceMemory m_handle;

    const Device& m_device;

    VkMemoryRequirements m_requirements;
    VkMemoryPropertyFlags m_property_flags;

  }; // class DeviceMemory
} // namespace comet