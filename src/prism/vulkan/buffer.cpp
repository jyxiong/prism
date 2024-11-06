#include "prism/vulkan/buffer.h"

#include "prism/vulkan/error.h"
#include "prism/vulkan/utils.h"
#include "prism/vulkan/command_buffer.h"

using namespace prism;

Buffer::Buffer(const Device &m_device, VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode mode)
    : m_device(m_device)
{
  VkBufferCreateInfo buffer_info{};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = size;
  buffer_info.usage = usage;
  buffer_info.sharingMode = mode;

  VK_CHECK(vkCreateBuffer(m_device.get_handle(), &buffer_info, nullptr, &m_handle));

  vkGetBufferMemoryRequirements(m_device.get_handle(), m_handle, &m_memory_requirements);
}

Buffer::~Buffer()
{
  if (m_handle != VK_NULL_HANDLE)
  {
    vkDestroyBuffer(m_device.get_handle(), m_handle, nullptr);
  }
}

VkBuffer Buffer::get_handle() const
{
  return m_handle;
}

const VkMemoryRequirements &Buffer::get_memory_requirements() const
{
  return m_memory_requirements;
}

VkDeviceAddress Buffer::get_device_address() const
{
  VkBufferDeviceAddressInfoKHR info{};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
  info.buffer = m_handle;
  return m_device.get_extension_functions().get_buffer_device_address(m_device.get_handle(), &info);
}

void Buffer::bind(const DeviceMemory &memory, VkDeviceSize offset)
{
  VK_CHECK(vkBindBufferMemory(m_device.get_handle(), m_handle, memory.get_handle(), offset));
}

void Buffer::upload(const CommandPool& cmd_pool, const void *data, VkDeviceSize size, VkDeviceSize offset)
{
  auto stage_buffer = Buffer(m_device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  auto stage_memory = DeviceMemory(m_device, stage_buffer.get_memory_requirements(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  stage_buffer.bind(stage_memory);
  stage_memory.upload(0, size, data);

  auto queue_family_index = m_device.get_physical_device().get_queue_family_index(VK_QUEUE_GRAPHICS_BIT);
  auto &queue = m_device.get_queue(queue_family_index, 0);

  utils::submit_commands_to_queue(cmd_pool, queue, [&](const CommandBuffer &cmd_buffer) {
    VkBufferCopy copy_region{};
    copy_region.size = size;
    cmd_buffer.copy_buffer(stage_buffer, *this, {copy_region});
  });
}
