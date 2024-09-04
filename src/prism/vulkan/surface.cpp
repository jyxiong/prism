#include "prism/vulkan/surface.h"

using namespace prism;

Surface::Surface(const Instance& instance, const Window& window)
  : m_instance(instance)
{
  m_handle = window.create_surface(m_instance);
}

Surface::~Surface()
{
  if (m_handle)
  {
    vkDestroySurfaceKHR(m_instance.get_handle(), m_handle, nullptr);
  }
}

VkSurfaceKHR Surface::get_handle() const
{
  return m_handle;
}