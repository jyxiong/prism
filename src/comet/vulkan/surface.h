#pragma once


#include "comet/platform/window.h"
#include "comet/vulkan/instance.h"

namespace comet
{
  class Surface
  {
  public:
    Surface(const Instance& instance, const std::unique_ptr<Window>& window);

    ~Surface();

    VkSurfaceKHR get_handle() const;

  private:
    VkSurfaceKHR m_handle;

    const Instance& m_instance;
  };
}