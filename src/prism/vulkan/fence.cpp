#include "prism/vulkan/fence.h"

using namespace prism;

Fence::Fence(const Device& device, VkFenceCreateFlags flags)
  : m_device(device)
{
  VkFenceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  create_info.flags = flags;

  VK_CHECK(vkCreateFence(device.get_handle(), &create_info, nullptr, &m_handle));
}

Fence::Fence(Fence&& other) noexcept
  : m_handle(std::exchange(other.m_handle, VK_NULL_HANDLE)),
    m_device(other.m_device)
{
}

Fence::~Fence()
{
  if (m_handle != VK_NULL_HANDLE)
  {
    vkDestroyFence(m_device.get_handle(), m_handle, nullptr);
  }
}

void Fence::wait(uint64_t timeout)
{
  VK_CHECK(vkWaitForFences(m_device.get_handle(), 1, &m_handle, VK_TRUE, timeout));
}

void Fence::reset()
{
  VK_CHECK(vkResetFences(m_device.get_handle(), 1, &m_handle));
}

VkFence Fence::get_handle() const
{
  return m_handle;
}