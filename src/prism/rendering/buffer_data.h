#pragma once

#include "prism/vulkan/buffer.h"
#include "prism/vulkan/device.h"
#include "prism/vulkan/device_memory.h"
#include "prism/vulkan/descriptor.h"

namespace prism
{
  struct BufferData
  {
    std::unique_ptr<Buffer> buffer;
    std::unique_ptr<DeviceMemory> device_memory;
    VkDeviceSize size;

    BufferData(const Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    
    ~BufferData();

    BufferData(const BufferData&) = delete;

    BufferData& operator=(const BufferData&) = delete;

    BufferData(BufferData&& other) = default;

    BufferData& operator=(BufferData&& other) = delete;

    BufferInfo get_info() const;

    void upload(const void* data, VkDeviceSize size, VkDeviceSize offset = 0);
  
    void upload(const CommandPool& cmd_pool, const void* data, VkDeviceSize size, VkDeviceSize offset = 0);

  }; // struct BufferData

  struct UniformBuffer : public BufferData
  {
    UniformBuffer(const Device& device, VkDeviceSize size);

    void upload(const void* data, VkDeviceSize size);

  }; // struct UniformBuffer

  struct VertexBuffer : public BufferData
  {
    VertexBuffer(const Device& device, VkDeviceSize size);

    void upload(const CommandPool& cmd_pool, const void* data, VkDeviceSize size);
  }; // struct VertexBuffer

  struct IndexBuffer : public BufferData
  {
    IndexBuffer(const Device& device, VkDeviceSize size);

    void upload(const CommandPool& cmd_pool, const void* data, VkDeviceSize size);
  }; // struct IndexBuffer

} // namespace prism