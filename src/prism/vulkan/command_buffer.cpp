#include "prism/vulkan/command_buffer.h"

#include "prism/vulkan/error.h"
#include "prism/vulkan/command_pool.h"

using namespace prism;

CommandBuffer::CommandBuffer(const CommandPool &cmd_pool, VkCommandBufferLevel level)
    : m_device(cmd_pool.get_device()), m_cmd_pool(cmd_pool)
{
  VkCommandBufferAllocateInfo allocate_info = {};
  allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocate_info.commandPool = m_cmd_pool.get_handle();
  allocate_info.level = level;
  allocate_info.commandBufferCount = 1;

  VK_CHECK(vkAllocateCommandBuffers(m_device.get_handle(), &allocate_info, &m_handle));
}

CommandBuffer::CommandBuffer(CommandBuffer &&other) noexcept
    : m_handle(std::exchange(other.m_handle, VK_NULL_HANDLE)),
      m_device(other.m_device),
      m_cmd_pool(other.m_cmd_pool)
{
}

CommandBuffer::~CommandBuffer()
{
  if (m_handle != VK_NULL_HANDLE)
  {
    vkFreeCommandBuffers(m_device.get_handle(), m_cmd_pool.get_handle(), 1, &m_handle);
  }
}

const VkCommandBuffer &CommandBuffer::get_handle() const
{
  return m_handle;
}

void CommandBuffer::begin(VkCommandBufferUsageFlags flags)
{
  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = flags;

  VK_CHECK(vkBeginCommandBuffer(m_handle, &begin_info));
}

void CommandBuffer::end()
{
  VK_CHECK(vkEndCommandBuffer(m_handle));
}

void CommandBuffer::reset()
{
  VK_CHECK(vkResetCommandBuffer(m_handle, 0));
}