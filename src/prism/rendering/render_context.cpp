#include "prism/rendering/render_context.h"

using namespace prism;

RenderContext::RenderContext(const Window &window, const Surface &surface,
                             const Device &device, const Queue &queue)
    : m_surface(surface), m_device(device), m_queue(queue),
      m_extent{window.get_extent().x, window.get_extent().y} {

  create_swapchain();

  create_render_frames();

  create_sync_objects();
}

VkFormat RenderContext::get_format() const { return m_swapchain->get_format(); }

const std::vector<RenderFrame> &RenderContext::get_render_frames() const {
  return m_render_frames;
}

uint32_t RenderContext::get_active_frame_index() const {
  return m_active_frame_index;
}

RenderFrame &RenderContext::get_active_frame() {
  return m_render_frames[m_active_frame_index];
}

void RenderContext::update(const VkExtent2D &extent) {
  m_extent = extent;

  recreate();
}

VkResult RenderContext::prepare_frame() {
  auto result = m_swapchain->acquire_next_image(
      UINT64_MAX, *m_image_availabel_semaphores, m_active_frame_index);

  m_render_frames[m_active_frame_index].reset();

  return result;
}

void RenderContext::render(
    const CommandBuffer &cmd_buffer,
    const std::function<void(const CommandBuffer &cmd_buffer)> &record_func) {
  auto &frame = m_render_frames[m_active_frame_index];

  cmd_buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  record_func(cmd_buffer);

  cmd_buffer.end();

  // queue.submit(cmd_buffer);

  // submit command buffer
  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &cmd_buffer.get_handle();
  // wait semaphore
  VkSemaphore wait_semaphores[] = {m_image_availabel_semaphores->get_handle()};
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_semaphores;
  // signal semaphore
  VkSemaphore signal_semaphores = frame.get_semaphore().get_handle();
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = &signal_semaphores;
  // wait stage
  VkPipelineStageFlags wait_stages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit_info.pWaitDstStageMask = wait_stages;

  m_queue.submit(submit_info, frame.get_fence());
  m_queue.wait_idle();
}

VkResult RenderContext::present_frame() {
  auto &frame = m_render_frames[m_active_frame_index];

  VkSemaphore signal_semaphores = frame.get_semaphore().get_handle();

  // present
  VkPresentInfoKHR present_info{};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = &signal_semaphores;
  VkSwapchainKHR swapchains[] = {m_swapchain->get_handle()};
  present_info.swapchainCount = 1;
  present_info.pSwapchains = swapchains;
  present_info.pImageIndices = &m_active_frame_index;

  return m_queue.present(present_info);
}

void RenderContext::create_swapchain() {
  Swapchain::Properties props{};
  props.extent = m_extent;
  props.image_usage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  props.surface_format = {VK_FORMAT_B8G8R8A8_UNORM,
                          VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
  m_swapchain = std::make_unique<Swapchain>(m_device, m_surface, props);
}

void RenderContext::create_render_frames() {
  m_render_frames.reserve(m_swapchain->get_images().size());
  for (size_t i = 0; i < m_swapchain->get_images().size(); ++i) {
    m_render_frames.emplace_back(m_device, m_swapchain->get_images()[i]);
  }
}

void RenderContext::create_sync_objects() {
  m_image_availabel_semaphores = std::make_unique<Semaphore>(m_device);
}

void RenderContext::recreate() {

  // TODO: implement update render target in render frame
  m_image_availabel_semaphores.reset();
  m_render_frames.clear();
  m_swapchain.reset();

  create_swapchain();
  create_render_frames();
  create_sync_objects();
}
