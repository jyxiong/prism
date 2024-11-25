#include "renderer.h"

#include "stb_image_write.h"

#include "prism/platform/glfw_window.h"
#include "prism/vulkan/device_features.h"
#include "prism/vulkan/utils.h"
#include "prism/vulkan/shader_stage.h"

Renderer::Renderer() {
  create_window();
  create_instance();
  create_device();
  create_surface();
  create_swapchain();

  create_render_frame();
  create_sync_object();

  create_render_pass();
  create_pipeline();
  create_framebuffer();
  // create_descriptors();
}

bool Renderer::prepare() {
  return true;
}

void Renderer::update() {
  uint32_t image_index;
  auto result = aquire_image(&image_index);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    resize();
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swapchain image!");
  }

  render_image(image_index);

  result = present_image(image_index);

  const auto &extent = m_window->get_extent();
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_extent.width != extent.x || m_extent.height != extent.y) {
    resize();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swapchain image!");
  }
}

bool Renderer::resize() {
  m_device->wait_idle();

  glfwGetFramebufferSize((GLFWwindow *)m_window->get_handle(), (int*)&m_extent.width, (int*)&m_extent.height);
  while (m_extent.width == 0 || m_extent.height == 0) {
      glfwGetFramebufferSize((GLFWwindow *)m_window->get_handle(), (int*)&m_extent.width, (int*)&m_extent.height);
      glfwWaitEvents();
  }

  m_framebuffers.clear();
  m_render_frames.clear();
  m_swapchain.reset();

  create_swapchain();
  create_render_frame();
  create_framebuffer();

  return true;
}

void Renderer::render_loop() {

  while (!m_window->should_close()) {
    m_window->process_events();

    update();
  }

  m_device->wait_idle();
}

void Renderer::create_window() {
  Window::Properties props{};
  props.extent = { m_extent.width, m_extent.height};
  props.resizable = true;
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

  LOG_INFO("Vulkan instance created");
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
      VK_QUEUE_TRANSFER_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT);

  LOG_INFO("Vulkan device created");
}

void Renderer::create_surface() {
  m_surface = std::make_unique<Surface>(*m_instance, *m_window);
}

void Renderer::create_swapchain() {
  Swapchain::Properties props{};
  props.extent = m_extent;
  props.image_usage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  props.surface_format = {VK_FORMAT_B8G8R8A8_UNORM,
                          VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
  m_swapchain = std::make_unique<Swapchain>(*m_device, *m_surface, props);
}

void Renderer::create_render_frame() {
  m_render_frames.reserve(m_swapchain->get_images().size());
  for (size_t i = 0; i < m_swapchain->get_images().size(); ++i) {
    m_render_frames.emplace_back(*m_device, m_swapchain->get_images()[i]);
  }
}

void Renderer::create_render_pass() {
  std::vector<AttachmentDescription> color_attachments(1);
  color_attachments[0].format = m_swapchain->get_format();
  color_attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

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
  m_framebuffers.reserve(m_swapchain->get_images().size());
  for (const auto &render_frame : m_render_frames) {
    m_framebuffers.emplace_back(*m_device, *m_render_pass, render_frame.get_image_views(), m_extent.width, m_extent.height);
  }
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
  m_image_availabel_semaphores = std::make_unique<Semaphore>(*m_device);
}

void Renderer::render_image(uint32_t image_index)
{
  auto& frame = m_render_frames[image_index];
  auto& cmd_buffer = frame.request_command_buffer(
      m_device->get_queue(m_queue_family_index, 0));

  cmd_buffer.reset();

  cmd_buffer.begin(
      VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  // render pass begin
  VkRenderPassBeginInfo render_pass_bi{};
  render_pass_bi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_bi.renderPass = m_render_pass->get_handle();
  render_pass_bi.framebuffer = m_framebuffers[image_index].get_handle();
  render_pass_bi.renderArea.offset = {0, 0};
  render_pass_bi.renderArea.extent = m_extent;
  
  std::array<VkClearValue, 1> clear_values{};
  clear_values[0].color = {0.0f, 0.0f, 0.0f, 1.0f};

  render_pass_bi.clearValueCount = clear_values.size();
  render_pass_bi.pClearValues = clear_values.data();

  cmd_buffer.begin_render_pass(
      render_pass_bi, VK_SUBPASS_CONTENTS_INLINE);

  cmd_buffer.bind_pipeline(*m_graphic_pipeline);

  // cmd_buffer.bind_descriptor_set(
  //     VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline_layout->get_handle(),
  //     m_descriptor_set->get_handle());

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(m_extent.width);
  viewport.height = static_cast<float>(m_extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  cmd_buffer.set_viewport(viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = m_extent;
  cmd_buffer.set_scissor(scissor);

  cmd_buffer.draw(3, 1, 0, 0);

  cmd_buffer.end_render_pass();

  cmd_buffer.end();

  auto &queue = m_device->get_queue(m_queue_family_index, 0);
  // queue.submit(cmd_buffer);

  // submit command buffer
  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers =
      &cmd_buffer.get_handle();
  // wait semaphore
  VkSemaphore wait_semaphores[] = {
      m_image_availabel_semaphores->get_handle()};
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_semaphores;
  // signal semaphore
  VkSemaphore signal_semaphores = frame.get_semaphore().get_handle();
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = &signal_semaphores;
  // wait stage
  VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit_info.pWaitDstStageMask = wait_stages;

  if (vkQueueSubmit(queue.get_handle(), 1, &submit_info,
                    frame.get_fence().get_handle()) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  queue.wait_idle();
}

VkResult Renderer::present_image(uint32_t image_index)
{
  auto& frame = m_render_frames[image_index];

  VkSemaphore signal_semaphores = frame.get_semaphore().get_handle();

  // present
  VkPresentInfoKHR present_info{};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = &signal_semaphores;
  VkSwapchainKHR swapchains[] = {m_swapchain->get_handle()};
  present_info.swapchainCount = 1;
  present_info.pSwapchains = swapchains;
  present_info.pImageIndices = &image_index;

  auto &queue = m_device->get_queue(m_queue_family_index, 0);
  auto result = vkQueuePresentKHR(queue.get_handle(), &present_info);

  return result;
}

VkResult Renderer::aquire_image(uint32_t *image_idx) {

  auto result = vkAcquireNextImageKHR(
      m_device->get_handle(), m_swapchain->get_handle(), UINT64_MAX,
      m_image_availabel_semaphores->get_handle(),
      VK_NULL_HANDLE, image_idx);

  auto& frame = m_render_frames[*image_idx];
  frame.get_fence().wait();
  frame.get_fence().reset();

  return result;
}

