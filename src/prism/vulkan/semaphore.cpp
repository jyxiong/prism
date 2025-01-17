#include "prism/vulkan/semaphore.h"

using namespace prism;

Semaphore::Semaphore(const Device& device)
  : m_device(device)
{
  VkSemaphoreCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  create_info.flags = 0;

  VK_CHECK(vkCreateSemaphore(device.get_handle(), &create_info, nullptr, &m_handle));
}

Semaphore::Semaphore(Semaphore&& other) noexcept
  : m_handle(std::exchange(other.m_handle, VK_NULL_HANDLE)),
    m_device(other.m_device)
{
}

Semaphore::~Semaphore()
{
  if (m_handle != VK_NULL_HANDLE)
  {
    vkDestroySemaphore(m_device.get_handle(), m_handle, nullptr);
  }
}

VkSemaphore Semaphore::get_handle() const
{
  return m_handle;
}