#pragma once

#include "prism/vulkan/device.h"

namespace prism
{
  class Fence
  {
  public:
    Fence(const Device& device, VkFenceCreateFlags flags = 0);

    ~Fence();

    void wait(uint64_t timeout = UINT64_MAX);

    void reset();

    VkFence get_handle() const;

  private:
    VkFence m_handle;

    const Device& m_device;
  };
}