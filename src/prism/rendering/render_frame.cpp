#include "prism/rendering/render_frame.h"

using namespace prism;

RenderFrame::RenderFrame(const Device &device, const Image &image)
    : m_device(device) {
  ImageViewCreateInfo image_view_ci{};
  image_view_ci.set_view_type(VK_IMAGE_VIEW_TYPE_2D)
      .set_level_count(1)
      .set_layer_count(1)
      .set_aspect_mask(VK_IMAGE_ASPECT_COLOR_BIT);
  image_view_ci.set_format(image.get_format());
  m_image_views.emplace_back(image, image_view_ci);

  m_semaphore = std::make_unique<Semaphore>(m_device);
  m_fence = std::make_unique<Fence>(m_device, VK_FENCE_CREATE_SIGNALED_BIT);
}

RenderFrame::RenderFrame(RenderFrame && other)
    : m_device(other.m_device),
      m_image_views(std::move(other.m_image_views)),
      m_cmd_pools(std::move(other.m_cmd_pools)),
      m_descriptor_pool(std::move(other.m_descriptor_pool)),
      m_descriptor_set(std::move(other.m_descriptor_set)),
      m_semaphore(std::move(other.m_semaphore)),
      m_fence(std::move(other.m_fence))
{
}

RenderFrame::~RenderFrame() {}

const std::vector<ImageView> &RenderFrame::get_image_views() const {
  return m_image_views;
}

const Semaphore &RenderFrame::get_semaphore() const { return *m_semaphore; }

Fence &RenderFrame::get_fence() const { return *m_fence; }

CommandBuffer &RenderFrame::request_command_buffer(const Queue &queue) {
  auto &cmd_pool = get_command_pool(queue);
  return cmd_pool.request_command_buffer();
}

CommandPool &RenderFrame::get_command_pool(const Queue &queue) {
  auto cmd_pool_it = m_cmd_pools.find(queue.get_family_index());
  if (cmd_pool_it == m_cmd_pools.end()) {
    m_cmd_pools[queue.get_family_index()] = std::make_unique<CommandPool>(
        m_device, queue.get_family_index(),
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  }
  return *m_cmd_pools[queue.get_family_index()];
}