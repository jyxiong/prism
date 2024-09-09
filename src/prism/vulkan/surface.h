#pragma once

#include "prism/platform/window.h"
#include "prism/vulkan/instance.h"

namespace prism
{
  class Surface
  {
  public:
    Surface(const Instance& instance, const Window& window);

    Surface(const Surface&) = delete;

    Surface(Surface&& other) = delete;

    ~Surface();

    Surface& operator=(const Surface&) = delete;

    Surface& operator=(Surface&&) = delete;

    VkSurfaceKHR get_handle() const;

  private:
    VkSurfaceKHR m_handle;

    const Instance& m_instance;
  };
}