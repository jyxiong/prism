#pragma once

#include "prism/vulkan/device.h"

namespace prism
{
  class Semaphore
  {
  public:
    Semaphore(const Device& device);

    ~Semaphore();

    VkSemaphore get_handle() const;

  private:
    VkSemaphore m_handle;

    const Device& m_device;
  };
}