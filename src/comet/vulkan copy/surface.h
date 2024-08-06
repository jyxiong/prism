#pragma once

#include "vulkan/vulkan.h"
#include "comet/platform/window.h"
#include "comet/vulkan/instance.h"

namespace comet
{
  class Surface
  {
  public:
    Surface(const Instance& instance, const Window& window);

    ~Surface();

    VkSurfaceKHR get_handle() const;

  private:
    VkSurfaceKHR m_handle;

    const Instance& m_instance;
  };
}