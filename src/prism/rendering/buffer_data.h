#pragma once

#include "prism/vulkan/buffer.h"
#include "prism/vulkan/device.h"
#include "prism/vulkan/device_memory.h"

namespace prism
{
  struct BufferData
  {
    std::unique_ptr<Buffer> buffer;
    std::unique_ptr<DeviceMemory> device_memory;

    BufferData(const Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    
    ~BufferData();

    BufferData(const BufferData&) = delete;

    BufferData& operator=(const BufferData&) = delete;

    BufferData(BufferData&& other) = default;

    BufferData& operator=(BufferData&& other) = delete;

    void upload(const void* data, VkDeviceSize size, VkDeviceSize offset = 0);
  
    void upload(const CommandPool& cmd_pool, const void* data, VkDeviceSize size, VkDeviceSize offset = 0);

  }; // struct BufferData
} // namespace prism