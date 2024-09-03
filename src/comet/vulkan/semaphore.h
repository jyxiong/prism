#pragma once

#include "comet/vulkan/device.h"

namespace comet
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