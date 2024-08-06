#include "comet/vulkan/command_pool.h"

#include "comet/vulkan/error.h"

using namespace comet;

CommandPool::CommandPool(const Device &device, uint32_t queue_family_index, VkCommandPoolCreateFlags flags)
    : m_device(device)
{
  VkCommandPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.queueFamilyIndex = queue_family_index;
  pool_info.flags = flags;

  VK_CHECK(vkCreateCommandPool(m_device.get_handle(), &pool_info, nullptr, &m_handle));
}

CommandPool::~CommandPool()
{
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