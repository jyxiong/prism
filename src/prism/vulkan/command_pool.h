#pragma once

#include "prism/vulkan/device.h"

namespace prism
{
  class CommandBuffer;

  class CommandPool
  {
  public:
    CommandPool(const Device &device, uint32_t queue_family_index, VkCommandPoolCreateFlags flags = 0);

    CommandPool(const CommandPool &) = delete;

    CommandPool(CommandPool &&other) noexcept;

    ~CommandPool();

    CommandPool& operator=(const CommandPool &) = delete;

    CommandPool& operator=(CommandPool &&) = delete;

    VkCommandPool get_handle() const;

    const Device &get_device() const;

  private:
    VkCommandPool m_handle;

    const Device& m_device;
  };

} // namespace prism