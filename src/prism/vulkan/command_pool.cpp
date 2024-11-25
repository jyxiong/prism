#include "prism/vulkan/command_pool.h"

#include "prism/vulkan/command_buffer.h"

using namespace prism;

CommandPool::CommandPool(const Device &device, uint32_t queue_family_index, VkCommandPoolCreateFlags flags)
    : m_device(device)
{
  VkCommandPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.queueFamilyIndex = queue_family_index;
  pool_info.flags = flags;

  VK_CHECK(vkCreateCommandPool(m_device.get_handle(), &pool_info, nullptr, &m_handle));
}

CommandPool::CommandPool(CommandPool &&other) noexcept
    : m_handle(std::exchange(other.m_handle, VK_NULL_HANDLE)),
      m_device(other.m_device)
{
}

CommandPool::~CommandPool()
{
  m_cmd_buffers.clear();

  if (m_handle != VK_NULL_HANDLE)
  {
    vkDestroyCommandPool(m_device.get_handle(), m_handle, nullptr);
  }
}

VkCommandPool CommandPool::get_handle() const
{
  return m_handle;
}

const Device &CommandPool::get_device() const
{
  return m_device;
}

CommandBuffer &CommandPool::request_command_buffer(VkCommandBufferLevel level)
{
  if (m_active_cmd_buffer_count == m_cmd_buffers.size())
  {
    m_cmd_buffers.emplace_back(std::make_unique<CommandBuffer>(*this, level));
  }

  return *m_cmd_buffers[m_active_cmd_buffer_count++];
}

void CommandPool::reset()
{
  VK_CHECK(vkResetCommandPool(m_device.get_handle(), m_handle, 0));

  reset_command_buffers();
}

void CommandPool::clear_command_buffers()
{
  m_cmd_buffers.clear();
  m_active_cmd_buffer_count = 0;
}

void CommandPool::reset_command_buffers()
{
  for (auto &cmd_buffer : m_cmd_buffers)
  {
    cmd_buffer->reset();
  }

  m_active_cmd_buffer_count = 0;
}