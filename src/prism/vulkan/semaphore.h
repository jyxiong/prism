#pragma once

#include "prism/vulkan/device.h"

namespace prism
{
  class Semaphore
  {
  public:
    Semaphore(const Device& device);

    Semaphore(const Semaphore&) = delete;

    Semaphore(Semaphore&& other) noexcept;

    ~Semaphore();

    Semaphore& operator=(const Semaphore&) = delete;

    Semaphore& operator=(Semaphore&&) = delete;

    VkSemaphore get_handle() const;

  private:
    VkSemaphore m_handle;

    const Device& m_device;
  };
}