#include "renderer.h"

#include "stb_image_write.h"

#include "prism/platform/glfw_window.h"
#include "prism/vulkan/device_features.h"
#include "prism/vulkan/utils.h"

static uint32_t WIDTH = 800;
static uint32_t HEIGHT = 600;

static std::vector<glm::u8vec4> red(WIDTH *HEIGHT, glm::u8vec4(255, 0, 0, 255));
static std::vector<glm::u8vec4> green(WIDTH *HEIGHT,
                                      glm::u8vec4(0, 255, 0, 255));
static std::vector<glm::u8vec4> blue(WIDTH *HEIGHT,
                                     glm::u8vec4(0, 0, 255, 255));
static uint32_t frame_count = 0;

Renderer::Renderer() {
  create_window();
  create_instance();
  create_device();
  create_surface();
  create_swapchain();
  create_render_target();
  create_command_pool();
  create_command_buffer();
  create_sync_object();
}

void Renderer::render() {

  while (!m_window->should_close()) {
    m_window->process_events();

    draw();
  }

  m_device->wait_idle();
}

void Renderer::create_window() {
  Window::Properties props{};
  props.extent = {WIDTH, HEIGHT};
  props.resizable = false;
  props.title = "Prism";
  m_window = std::make_unique<GlfwWindow>(props);
}

void Renderer::create_instance() {
  auto window_exts = m_window->get_required_extensions();

  Instance::ExtensionNames inst_exts{};
  inst_exts.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  inst_exts.insert(inst_exts.end(), window_exts.begin(), window_exts.end());

  Instance::LayerNames inst_layers{};
  inst_layers.push_back("VK_LAYER_KHRONOS_validation");

  m_instance = std::make_unique<Instance>(inst_exts, inst_layers);
}

void Renderer::create_device() {
  Device::ExtensionNames dev_exts{};
  dev_exts.push_back(VK_KHR_RAY_QUERY_EXTENSION_NAME);
  dev_exts.push_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
  dev_exts.push_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
  dev_exts.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  DeviceFeatures dev_features{};
  dev_features.request<VkPhysicalDeviceAccelerationStructureFeaturesKHR>(
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR,
      &VkPhysicalDeviceAccelerationStructureFeaturesKHR::accelerationStructure);
  dev_features.request<VkPhysicalDeviceRayQueryFeaturesKHR>(
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR,
      &VkPhysicalDeviceRayQueryFeaturesKHR::rayQuery);
  m_device = std::make_unique<Device>(m_instance->pick_physical_device(),
                                      dev_exts, dev_features);
  m_queue_family_index = m_device->get_physical_device().get_queue_family_index(
      VK_QUEUE_TRANSFER_BIT);
}

void Renderer::create_surface() {
  m_surface = std::make_unique<Surface>(*m_instance, *m_window);
}

void Renderer::create_swapchain() {

  glfwGetFramebufferSize((GLFWwindow*)m_window->get_handle(), (int*)&WIDTH, (int*)&HEIGHT);

  Swapchain::Properties props{};
  props.extent = {WIDTH, HEIGHT};
  props.image_usage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  props.surface_format = {VK_FORMAT_B8G8R8A8_UNORM,
                          VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
  m_swapchain = std::make_unique<Swapchain>(*m_device, *m_surface, props);
}

void Renderer::create_render_target() {
  ImageCreateInfo image_ci{};
  image_ci.set_extent({WIDTH, HEIGHT, 1})
      .set_image_type(VK_IMAGE_TYPE_2D)
      .set_format(VK_FORMAT_R8G8B8A8_UNORM)
      .set_usage(VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
  m_storage_image = std::make_unique<Image>(*m_device, image_ci);

  m_storage_memory = std::make_unique<DeviceMemory>(
      *m_device, m_storage_image->get_memory_requirements(),
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  m_storage_image->bind(*m_storage_memory);

  ImageViewCreateInfo image_view_ci{};
  image_view_ci.set_view_type(VK_IMAGE_VIEW_TYPE_2D);
  m_storage_image_view =
      std::make_unique<ImageView>(*m_storage_image, image_view_ci);

  VkImageMemoryBarrier image_memory_barrier{};
  image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
  image_memory_barrier.srcAccessMask = 0;
  image_memory_barrier.dstAccessMask = 0;
  image_memory_barrier.image = m_storage_image->get_handle();
  image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  image_memory_barrier.subresourceRange.layerCount = 1;
  image_memory_barrier.subresourceRange.levelCount = 1;

  // auto& queue = m_device->get_queue(m_queue_family_index, 0);
  // utils::submit_commands_to_queue(*m_command_pool, queue, [&](const
  // CommandBuffer &cmd) {
  //   vkCmdPipelineBarrier(cmd.get_handle(), VK_,
  //                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr,
  //                        0, nullptr, 1, &image_memory_barrier);
  // });
}

void Renderer::create_command_pool() {
  m_command_pool = std::make_unique<CommandPool>(
      *m_device, m_queue_family_index,
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
}

void Renderer::create_command_buffer() {
  m_command_buffers.reserve(m_swapchain->get_images().size());
  for (size_t i = 0; i < m_swapchain->get_images().size(); ++i) {
    m_command_buffers.emplace_back(*m_command_pool);
  }
}

void Renderer::create_sync_object() {
  auto swapchain_image_count = m_swapchain->get_images().size();

  m_image_availabel_semaphores.reserve(swapchain_image_count);
  m_render_finished_semaphores.reserve(swapchain_image_count);
  m_in_flight_fences.reserve(swapchain_image_count);

  for (size_t i = 0; i < swapchain_image_count; ++i) {
    m_image_availabel_semaphores.emplace_back(*m_device);
    m_render_finished_semaphores.emplace_back(*m_device);
    m_in_flight_fences.emplace_back(*m_device, VK_FENCE_CREATE_SIGNALED_BIT);
  }
}

void Renderer::draw() {
  m_in_flight_fences[m_current_frame].wait();

  uint32_t image_index;
  auto result = vkAcquireNextImageKHR(
      m_device->get_handle(), m_swapchain->get_handle(), UINT64_MAX,
      m_image_availabel_semaphores[m_current_frame].get_handle(),
      VK_NULL_HANDLE, &image_index);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreate_swapchain();
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swapchain image!");
  }

  m_in_flight_fences[m_current_frame].reset();

  m_command_buffers[m_current_frame].reset();

  m_command_buffers[m_current_frame].begin(
      VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  // darw to storage image
  auto draw_to_storage_image = [&](const std::vector<glm::u8vec4> &data) {
    m_storage_image->upload(*m_command_pool, data.data(),
                            data.size() * sizeof(glm::u8vec4));
  };

  if (frame_count % 30 < 10) {
    draw_to_storage_image(red);
  } else if (frame_count % 30 > 20) {
    draw_to_storage_image(green);
  } else {
    draw_to_storage_image(blue);
  }

  frame_count++;

  // transition storage image layout
  VkImageMemoryBarrier image_memory_barrier{};
  image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  image_memory_barrier.subresourceRange.layerCount = 1;
  image_memory_barrier.subresourceRange.levelCount = 1;
  
  image_memory_barrier.srcAccessMask = 0;
  image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
  image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  image_memory_barrier.image = m_storage_image->get_handle();

  vkCmdPipelineBarrier(m_command_buffers[m_current_frame].get_handle(),
                       VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                       VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0,
                       nullptr, 1, &image_memory_barrier);

  // transition swapchain image layout
  auto &swapchain_image = m_swapchain->get_images()[m_current_frame];
  image_memory_barrier.srcAccessMask = 0;
  image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  image_memory_barrier.image = swapchain_image;

  vkCmdPipelineBarrier(m_command_buffers[m_current_frame].get_handle(),
                       VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                       VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0,
                       nullptr, 1, &image_memory_barrier);

  // copy storage image to swapchain image
  VkImageCopy image_copy{};
  image_copy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  image_copy.srcSubresource.layerCount = 1;
  image_copy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  image_copy.dstSubresource.layerCount = 1;
  image_copy.extent.width = WIDTH;
  image_copy.extent.height = HEIGHT;
  image_copy.extent.depth = 1;

  vkCmdCopyImage(m_command_buffers[m_current_frame].get_handle(),
                 m_storage_image->get_handle(),
                 VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, swapchain_image,
                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &image_copy);

  // transition swapchain image layout
  image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  image_memory_barrier.dstAccessMask = 0;
  image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  image_memory_barrier.image = swapchain_image;

  vkCmdPipelineBarrier(m_command_buffers[m_current_frame].get_handle(),
                       VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                       VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0,
                       nullptr, 1, &image_memory_barrier);

  // transition storage image layout
  image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
  image_memory_barrier.dstAccessMask = 0;
  image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
  image_memory_barrier.image = m_storage_image->get_handle();

  vkCmdPipelineBarrier(m_command_buffers[m_current_frame].get_handle(),
                       VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                       VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0,
                       nullptr, 1, &image_memory_barrier);

  m_command_buffers[m_current_frame].end();

  auto &queue = m_device->get_queue(m_queue_family_index, 0);
  // queue.submit(m_command_buffers[m_current_frame]);

  // submit command buffer
  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers =
      &m_command_buffers[m_current_frame].get_handle();
  // wait semaphore
  VkSemaphore wait_semaphores[] = {
      m_image_availabel_semaphores[m_current_frame].get_handle()};
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_semaphores;
  // signal semaphore
  VkSemaphore signal_semaphores[] = {
      m_render_finished_semaphores[m_current_frame].get_handle()};
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signal_semaphores;
  // wait stage
  VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_ALL_COMMANDS_BIT};
  submit_info.pWaitDstStageMask = wait_stages;

  if (vkQueueSubmit(queue.get_handle(), 1, &submit_info,
                    m_in_flight_fences[m_current_frame].get_handle()) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  // present
  VkPresentInfoKHR present_info{};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = signal_semaphores;
  VkSwapchainKHR swapchains[] = {m_swapchain->get_handle()};
  present_info.swapchainCount = 1;
  present_info.pSwapchains = swapchains;
  present_info.pImageIndices = &image_index;

  result = vkQueuePresentKHR(queue.get_handle(), &present_info);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    recreate_swapchain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swapchain image!");
  }

  m_current_frame = (m_current_frame + 1) % m_swapchain->get_images().size();
}

void Renderer::recreate_swapchain() {
  m_device->wait_idle();
  m_swapchain.reset();
  m_storage_image.reset();
  m_storage_memory.reset();
  m_storage_image_view.reset();

  create_swapchain();
  create_render_target();
}