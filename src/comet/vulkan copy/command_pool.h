#pragma once

#include "vulkan/vulkan.h"

#include "comet/vulkan/device.h"

namespace comet
{
  class CommandBuffer;

  class CommandPool
  {
  public:
    CommandPool(const Device &device, uint32_t queue_family_index, VkCommandPoolCreateFlags flags = 0);
    ~CommandPool();

    VkCommandPool get_handle() const;

    const Device &get_device() const;

  private:
    VkCommandPool m_handle;

    const Device& m_device;
  };

} // namespace comet