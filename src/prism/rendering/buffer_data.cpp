#include "prism/rendering/buffer_data.h"

#include "prism/vulkan/command_buffer.h"
#include "prism/vulkan/utils.h"

using namespace prism;

BufferData::BufferData(const Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
  : size(size)
{
  buffer = std::make_unique<Buffer>(device, size, usage);
  device_memory = std::make_unique<DeviceMemory>(device, buffer->get_memory_requirements(), properties);
  buffer->bind_memory(*device_memory);
}

BufferData::~BufferData()
{
  device_memory.reset();
  buffer.reset();
}

BufferInfo BufferData::get_info() const
{
  BufferInfo info{};
  info.buffer = buffer->get_handle();
  info.offset = 0;
  info.range = size;
  return info;
}

void BufferData::upload(const void* data, VkDeviceSize size, VkDeviceSize offset)
{
  device_memory->upload(offset, size, data);
}

void BufferData::upload(const CommandPool& cmd_pool, const void* data, VkDeviceSize size, VkDeviceSize offset)
{
  const auto& device = buffer->get_device();

  auto stage_buffer = Buffer(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  auto stage_memory = DeviceMemory(device, stage_buffer.get_memory_requirements(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  stage_buffer.bind_memory(stage_memory);
  stage_memory.upload(0, size, data);

  auto queue_family_index = device.get_physical_device().get_queue_family_index(VK_QUEUE_GRAPHICS_BIT);
  auto &queue = device.get_queue(queue_family_index, 0);
  utils::submit_commands_to_queue(cmd_pool, queue, [&](const CommandBuffer &cmd_buffer) {
    VkBufferCopy copy_region{};
    copy_region.size = size;
    cmd_buffer.copy_buffer(stage_buffer, *buffer, {copy_region});
  });
}

UniformBuffer::UniformBuffer(const Device& device, VkDeviceSize size)
  : BufferData(device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
{
}

void UniformBuffer::upload(const void* data, VkDeviceSize size)
{
  BufferData::upload(data, size);
}

VertexBuffer::VertexBuffer(const Device& device, VkDeviceSize size)
  : BufferData(device, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
{
}

void VertexBuffer::upload(const CommandPool& cmd_pool, const void* data, VkDeviceSize size)
{
  BufferData::upload(cmd_pool, data, size);
}

IndexBuffer::IndexBuffer(const Device& device, VkDeviceSize size)
  : BufferData(device, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
{
}

void IndexBuffer::upload(const CommandPool& cmd_pool, const void* data, VkDeviceSize size)
{
  BufferData::upload(cmd_pool, data, size);
}