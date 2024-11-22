#include "prism/vulkan/buffer.h"

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

const Device &Buffer::get_device() const
{
  return m_device;
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

void Buffer::bind_memory(const DeviceMemory &memory, VkDeviceSize offset)
{
  VK_CHECK(vkBindBufferMemory(m_device.get_handle(), m_handle, memory.get_handle(), offset));
}
