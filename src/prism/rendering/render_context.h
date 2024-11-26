#pragma once

#include "prism/platform/window.h"
#include "prism/rendering/render_frame.h"
#include "prism/vulkan/queue.h"
#include "prism/vulkan/device.h"
#include "prism/vulkan/surface.h"
#include "prism/vulkan/swapchain.h"

namespace prism
{

class RenderContext
{
public:
  RenderContext(const Window& window, const Surface &surface, const Device &device, const Queue& queue);

  VkFormat get_format() const;

  const std::vector<RenderFrame>& get_render_frames() const;

  RenderFrame& get_active_frame();

  uint32_t get_active_frame_index() const;

  void resize(const VkExtent2D& extent);

  VkResult aquire();

  void render(const CommandBuffer &cmd_buffer, const std::function<void(const CommandBuffer&)> &record_func);

  VkResult present();

private:
  void create_swapchain();

  void create_render_frames();

  void create_sync_objects();

private:
  const Surface &m_surface;

  const Device &m_device;

  const Queue &m_queue;

  VkExtent2D m_extent;

  std::unique_ptr<Swapchain> m_swapchain;

  uint32_t m_active_frame_index;

  std::vector<RenderFrame> m_render_frames;

  std::unique_ptr<Semaphore> m_image_availabel_semaphores;

}; // class RenderContext

} // namespace prism