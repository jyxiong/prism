#include "renderer.h"

#include "stb_image_write.h"

#include "prism/platform/glfw_window.h"
#include "prism/vulkan/device_features.h"
#include "prism/vulkan/utils.h"
#include "prism/vulkan/shader_stage.h"

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
  create_command_pool();
  create_command_buffer();
  create_render_target();
  create_sync_object();
  create_render_pass();
  create_pipeline();
  create_framebuffer();
  create_descriptors();
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

  glfwGetFramebufferSize((GLFWwindow *)m_window->get_handle(), (int *)&WIDTH,
                         (int *)&HEIGHT);

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
      .set_format(m_swapchain->get_format())
      .set_usage(VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                 VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
  ImageViewCreateInfo image_view_ci{};
  image_view_ci.set_view_type(VK_IMAGE_VIEW_TYPE_2D);

  m_storage_data = std::make_unique<ImageData>(*m_device, image_ci, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image_view_ci);

  auto &queue = m_device->get_queue(m_queue_family_index, 0);
  utils::submit_commands_to_queue(
      *m_command_pool, queue, [&](const CommandBuffer &cmd_buffer) {
        VkImageSubresourceRange subresource_range{};
        subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresource_range.baseMipLevel = 0;
        subresource_range.levelCount = 1;
        subresource_range.baseArrayLayer = 0;
        subresource_range.layerCount = 1;
        m_storage_data->image->set_layout(cmd_buffer, VK_IMAGE_LAYOUT_GENERAL,
                                          subresource_range);
      });
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

void Renderer::create_render_pass() {
  std::vector<AttachmentDescription> color_attachments(1);
  color_attachments[0].format = m_swapchain->get_format();
  color_attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachments[0].finalLayout = VK_IMAGE_LAYOUT_GENERAL;

  auto color_attachment_ref = AttachmentReference{};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  std::vector<SubpassDescription> subpasses(1);
  subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpasses[0].colorAttachmentCount = 1;
  subpasses[0].pColorAttachments = &color_attachment_ref;

  std::vector<SubpassDependency> dependencies(1);
  dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
  dependencies[0].dstSubpass = 0;
  dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies[0].srcAccessMask = 0;
  dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  m_render_pass = std::make_unique<RenderPass>(*m_device, color_attachments,
                                               subpasses, dependencies);
}

void Renderer::create_pipeline() {
  auto vert_module = ShaderModule(*m_device, "../shaders/shader.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
  auto frag_module = ShaderModule(*m_device, "../shaders/shader.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

  std::vector<ShaderStage> shader_stages(2);
  shader_stages[0].set_stage(vert_module.get_stage())
      .set_module(vert_module)
      .set_entry_point(vert_module.get_entry_point());

  shader_stages[1].set_stage(frag_module.get_stage())
      .set_module(frag_module)
      .set_entry_point(frag_module.get_entry_point());

  std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachments(1);
  color_blend_attachments[0].colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachments[0].blendEnable = VK_FALSE;
  ColorBlendState color_blend_state{};
  color_blend_state.set_attachments(color_blend_attachments);

  std::vector<VkDynamicState> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT,
                                                VK_DYNAMIC_STATE_SCISSOR};
  DynamicState dynamic_state{};
  dynamic_state.set_dynamic_states(dynamic_states);

  m_pipeline_layout = std::make_unique<PipelineLayout>(*m_device, std::vector<DescriptorSetLayout>());

  GraphicsPipelineCreateInfo pipeline_ci{};
  pipeline_ci.set_shader_stages(shader_stages)
      .set_layout(*m_pipeline_layout)
      .set_render_pass(*m_render_pass)
      .set_subpass(0)
      .set_color_blend_state(color_blend_state)
      .set_dynamic_state(dynamic_state)
      .set_tesellation_state(TessellationState{})
      .set_input_assembly_state(InputAssemblyState{})
      .set_depth_stencil_state(DepthStencilState{})
      .set_multisample_state(MultisampleState{})
      .set_rasterization_state(RasterizationState{})
      .set_viewport_state(ViewportState{})
      .set_vertex_input_state(VertexInputState{});
  m_graphic_pipeline = std::make_unique<GraphicsPipeline>(*m_device, pipeline_ci);
}

void Renderer::create_framebuffer() {
  m_framebuffer = std::make_unique<Framebuffer>(
      *m_device, *m_render_pass, *m_storage_data->image_view, WIDTH, HEIGHT);
}

void Renderer::create_descriptors() {
  // std::vector<VkDescriptorPoolSize> pool_sizes{
  //     {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1}};
  // m_descriptor_pool =
  //     std::make_unique<DescriptorPool>(*m_device, pool_sizes, 1);

  // m_descriptor_set = std::make_unique<DescriptorSet>(
  //     *m_device, *m_descriptor_set_layout, *m_descriptor_pool);

  // VkDescriptorImageInfo image_info{};
  // image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
  // image_info.imageView = m_storage_data->image_view->get_handle();
  // image_info.sampler = VK_NULL_HANDLE;

  // VkWriteDescriptorSet write{};
  // write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  // write.dstSet = m_descriptor_set->get_handle();
  // write.dstBinding = 0;
  // write.dstArrayElement = 0;
  // write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
  // write.descriptorCount = 1;
  // write.pImageInfo = &image_info;

  // vkUpdateDescriptorSets(m_device->get_handle(), 1, &write, 0, nullptr);
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

  // render pass begin
  VkRenderPassBeginInfo render_pass_bi{};
  render_pass_bi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_bi.renderPass = m_render_pass->get_handle();
  render_pass_bi.framebuffer = m_framebuffer->get_handle();
  render_pass_bi.renderArea.offset = {0, 0};
  render_pass_bi.renderArea.extent = {WIDTH, HEIGHT};

  std::array<VkClearValue, 1> clear_values{};
  clear_values[0].color = {0.0f, 0.0f, 0.0f, 1.0f};

  render_pass_bi.clearValueCount = clear_values.size();
  render_pass_bi.pClearValues = clear_values.data();

  m_command_buffers[m_current_frame].begin_render_pass(
      render_pass_bi, VK_SUBPASS_CONTENTS_INLINE);

  // darw to storage image
  m_command_buffers[m_current_frame].bind_pipeline(*m_graphic_pipeline);

  // m_command_buffers[m_current_frame].bind_descriptor_set(
  //     VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline_layout->get_handle(),
  //     m_descriptor_set->get_handle());

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(WIDTH);
  viewport.height = static_cast<float>(HEIGHT);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  m_command_buffers[m_current_frame].set_viewport(viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = {WIDTH, HEIGHT};
  m_command_buffers[m_current_frame].set_scissor(scissor);

  m_command_buffers[m_current_frame].draw(3, 1, 0, 0);

  m_command_buffers[m_current_frame].end_render_pass();

  VkImageMemoryBarrier image_memory_barrier{};
  image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  image_memory_barrier.subresourceRange.layerCount = 1;
  image_memory_barrier.subresourceRange.levelCount = 1;

  // transition storage image layout to transfer source
  image_memory_barrier.srcAccessMask = 0;
  image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
  image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
  image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  image_memory_barrier.image = m_storage_data->image->get_handle();

  m_command_buffers[m_current_frame].pipeline_barrier(
      VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0,
      {}, {}, {image_memory_barrier});

  // transition swapchain image layout to transfer destination
  auto &swapchain_image = m_swapchain->get_images()[m_current_frame];
  image_memory_barrier.srcAccessMask = 0;
  image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  image_memory_barrier.image = swapchain_image.get_handle();

  m_command_buffers[m_current_frame].pipeline_barrier(
      VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0,
      {}, {}, {image_memory_barrier});

  // copy storage image to swapchain image
  VkImageCopy image_copy{};
  image_copy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  image_copy.srcSubresource.layerCount = 1;
  image_copy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  image_copy.dstSubresource.layerCount = 1;
  image_copy.extent.width = WIDTH;
  image_copy.extent.height = HEIGHT;
  image_copy.extent.depth = 1;

  m_command_buffers[m_current_frame].copy_image(*m_storage_data->image,
                                                swapchain_image, {image_copy});

  // transition swapchain image layout to present source
  image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  image_memory_barrier.dstAccessMask = 0;
  image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  image_memory_barrier.image = swapchain_image.get_handle();

  m_command_buffers[m_current_frame].pipeline_barrier(
      VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0,
      {}, {}, {image_memory_barrier});

  // transition storage image layout to general
  image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
  image_memory_barrier.dstAccessMask = 0;
  image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
  image_memory_barrier.image = m_storage_data->image->get_handle();

  m_command_buffers[m_current_frame].pipeline_barrier(
      VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0,
      {}, {}, {image_memory_barrier});

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
  m_storage_data.reset();

  create_swapchain();
  create_render_target();
}
