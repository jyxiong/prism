#pragma once


#include "prism/platform/window.h"
#include "prism/vulkan/instance.h"

namespace prism
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