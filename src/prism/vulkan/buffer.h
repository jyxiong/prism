#pragma once



#include "prism/vulkan/device.h"
#include "prism/vulkan/device_memory.h"
#include "prism/vulkan/command_pool.h"

namespace prism
{
  class Buffer
  {
  public:
    Buffer(const Device &device, VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode mode = VK_SHARING_MODE_EXCLUSIVE);

    ~Buffer();

    VkBuffer get_handle() const;

    const VkMemoryRequirements &get_memory_requirements() const;

    VkDeviceAddress get_device_address() const;

    void bind(const DeviceMemory &memory, VkDeviceSize offset = 0);

    void upload(const CommandPool& cmd_pool, const void *data, VkDeviceSize size, VkDeviceSize offset = 0);

  private:
    VkBuffer m_handle;

    const Device &m_device;

    VkMemoryRequirements m_memory_requirements;

  }; // class Buffer
} // namespace prism