#pragma once

#include "prism/vulkan/device.h"

namespace prism
{
  class DeviceMemory
  {
  public:
    DeviceMemory(const Device& device, const VkMemoryRequirements& requirements, VkMemoryPropertyFlags property_flags, VkMemoryAllocateFlags allocate_flags = 0);

    DeviceMemory(const DeviceMemory&) = delete;

    DeviceMemory(DeviceMemory&& other) noexcept;

    ~DeviceMemory();

    DeviceMemory& operator=(const DeviceMemory&) = delete;

    DeviceMemory& operator=(DeviceMemory&&) = delete;

    void upload(VkDeviceSize offset, VkDeviceSize size, const void* src_data);
    
    void download(VkDeviceSize offset, VkDeviceSize size, void* dst_data);

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
} // namespace prism