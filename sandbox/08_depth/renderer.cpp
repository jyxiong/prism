#include "renderer.h"

#include "stb_image_write.h"
#include "glm/gtc/matrix_transform.hpp"

#include "prism/platform/glfw_window.h"
#include "prism/rendering/utils.h"
#include "prism/vulkan/device_features.h"
#include "prism/vulkan/shader_stage.h"
#include "prism/vulkan/utils.h"

struct UniformMatrix
{
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static std::vector<VkVertexInputBindingDescription> getBindingDescription() {
        std::vector<VkVertexInputBindingDescription> bindingDescription(1);
        bindingDescription[0].binding = 0;
        bindingDescription[0].stride = sizeof(Vertex);
        bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }
};

const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4
};

Renderer::Renderer() {
  create_window();
  create_instance();
  create_device();
  create_surface();
  create_command_pool();

  create_render_context();

  create_texture();

  create_uniform_buffer();

  create_vertex_buffer();
  create_index_buffer();

  create_descriptor_layout();
  create_descriptors();

  create_render_pass();
  create_pipeline();
  create_framebuffer();
}

void Renderer::render_loop() {

  while (!m_window->should_close()) {
    m_window->process_events();

    auto result = m_render_context->prepare_frame();

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      resize();
      return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("failed to acquire swapchain image!");
    }

    update_uniform_buffer();
    render_image();

    result = m_render_context->present_frame();
    const auto &extent = m_window->get_extent();
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        m_extent.width != extent.x || m_extent.height != extent.y) {
      resize();
    } else if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to present swapchain image!");
    }
  }

  m_device->wait_idle();
}

void Renderer::create_window() {
  Window::Properties props{};
  props.extent = {m_extent.width, m_extent.height};
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

void Renderer::create_command_pool() {
  m_cmd_pool = std::make_unique<CommandPool>(*m_device, m_queue_family_index,
                                             VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
}

void Renderer::create_render_context() {
  m_render_context = std::make_unique<RenderContext>(*m_window, *m_surface,
                                                     *m_device,
                                                     m_device->get_queue(m_queue_family_index, 0));

  m_depth_attachment = std::make_unique<DepthAttachment>(*m_device, m_extent, VK_FORMAT_D16_UNORM);
}

void Renderer::create_descriptor_layout()
{
  DescriptorSetLayout::Bindings bindings{
    {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT},
    {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT}
  };

  m_descriptor_set_layout = std::make_unique<DescriptorSetLayout>(*m_device, bindings);
}

void Renderer::create_render_pass() {
  std::vector<AttachmentDescription> attachments(2);
  attachments[0].format = m_render_context->get_format();
  attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
  attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  attachments[1].format = m_depth_attachment->format;
  attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
  attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  auto color_attachment_ref = AttachmentReference{};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  auto depth_attachment_ref = AttachmentReference{};
  depth_attachment_ref.attachment = 1;
  depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  std::vector<SubpassDescription> subpasses(1);
  subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpasses[0].colorAttachmentCount = 1;
  subpasses[0].pColorAttachments = &color_attachment_ref;
  subpasses[0].pDepthStencilAttachment = &depth_attachment_ref;

  std::vector<SubpassDependency> dependencies(1);
  dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
  dependencies[0].dstSubpass = 0;
  dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies[0].srcAccessMask = 0;
  dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  m_render_pass = std::make_unique<RenderPass>(*m_device, attachments,
                                               subpasses, dependencies);
}

void Renderer::create_pipeline() {
  auto vert_module = ShaderModule(*m_device, "../shaders/shader.vert.spv",
                                  VK_SHADER_STAGE_VERTEX_BIT);
  auto frag_module = ShaderModule(*m_device, "../shaders/shader.frag.spv",
                                  VK_SHADER_STAGE_FRAGMENT_BIT);

  std::vector<ShaderStage> shader_stages(2);
  shader_stages[0]
      .set_stage(vert_module.get_stage())
      .set_module(vert_module)
      .set_entry_point(vert_module.get_entry_point());

  shader_stages[1]
      .set_stage(frag_module.get_stage())
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

  m_pipeline_layout = std::make_unique<PipelineLayout>(
      *m_device, *m_descriptor_set_layout);

  RasterizationState rasterization{};
  rasterization.set_cull_mode(VK_CULL_MODE_BACK_BIT)
      .set_front_face(VK_FRONT_FACE_COUNTER_CLOCKWISE)
      .set_polygon_mode(VK_POLYGON_MODE_FILL)
      .set_line_width(1.0f)
      .set_rasterizer_discard_enable(VK_FALSE);

  auto binding_descriptions = Vertex::getBindingDescription();
  auto attribute_descriptions = Vertex::getAttributeDescriptions();

  VertexInputState vertex_input_state{};
  vertex_input_state.set_binding_descriptions(binding_descriptions)
      .set_attribute_descriptions(attribute_descriptions);

  InputAssemblyState input_assembly{};
  input_assembly.set_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

  GraphicsPipelineCreateInfo pipeline_ci{};
  pipeline_ci.set_shader_stages(shader_stages)
      .set_layout(*m_pipeline_layout)
      .set_render_pass(*m_render_pass)
      .set_subpass(0)
      .set_color_blend_state(color_blend_state)
      .set_dynamic_state(dynamic_state)
      .set_tesellation_state(TessellationState{})
      .set_input_assembly_state(input_assembly)
      .set_depth_stencil_state(DepthStencilState{})
      .set_multisample_state(MultisampleState{})
      .set_rasterization_state(rasterization)
      .set_viewport_state(ViewportState{})
      .set_vertex_input_state(vertex_input_state);
  m_graphic_pipeline =
      std::make_unique<GraphicsPipeline>(*m_device, pipeline_ci);
}

void Renderer::create_framebuffer() {
  const auto &render_frames = m_render_context->get_render_frames();
  m_framebuffers.reserve(render_frames.size());
  for (const auto &render_frame : render_frames) {
    m_framebuffers.emplace_back(*m_device, *m_render_pass,
                                render_frame.get_image_views(), *m_depth_attachment->image_view, m_extent.width,
                                m_extent.height);
  }
}

void Renderer::create_descriptors() {
  const auto frame_count = m_render_context->get_render_frames().size();

  std::vector<VkDescriptorPoolSize> pool_sizes{
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (uint32_t)frame_count},
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, (uint32_t)frame_count}};
  m_descriptor_pool =
      std::make_unique<DescriptorPool>(*m_device, pool_sizes, (uint32_t)frame_count);

  m_descriptor_sets.reserve(frame_count);
  for (size_t i = 0; i < frame_count; i++) {
    m_descriptor_sets.emplace_back(*m_device, *m_descriptor_set_layout, *m_descriptor_pool);

    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = m_uniform_buffers[i].buffer->get_handle();
    buffer_info.offset = 0;
    buffer_info.range = sizeof(UniformMatrix);

    VkDescriptorImageInfo image_info{};
    image_info.imageLayout = m_texture->image->get_layout();
    image_info.imageView = m_texture->image_view->get_handle();
    image_info.sampler = m_texture->sampler->get_handle();
    
    std::vector<VkWriteDescriptorSet> writes(2);
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet = m_descriptor_sets[i].get_handle();
    writes[0].dstBinding = 0;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].descriptorCount = 1;
    writes[0].pBufferInfo = &buffer_info;

    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstSet = m_descriptor_sets[i].get_handle();
    writes[1].dstBinding = 1;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].descriptorCount = 1;
    writes[1].pImageInfo = &image_info;

    vkUpdateDescriptorSets(m_device->get_handle(), writes.size(), writes.data(), 0, nullptr);
  }
}

void Renderer::create_uniform_buffer()
{
  const auto frame_count = m_render_context->get_render_frames().size();
  m_uniform_buffers.reserve(frame_count);
  for (size_t i = 0; i < frame_count; i++)
  {
    m_uniform_buffers.emplace_back(*m_device, sizeof(UniformMatrix));
  }
}

void Renderer::create_texture()
{
  std::vector<glm::u8vec4> texture(256 * 256);
  for (size_t i = 0; i < 256; i++) {
    for (size_t j = 0; j < 256; j++) {
      texture[i * 256 + j] = glm::u8vec4(i, j, 0, 255);
    }
  }

  m_texture = std::make_unique<Texture>(*m_device, VkExtent2D{256, 256}, VK_FORMAT_R8G8B8A8_UNORM);
  m_texture->upload(*m_cmd_pool, texture.data(), 256 * 256 * sizeof(glm::u8vec4));

  auto &queue = m_device->get_queue(m_queue_family_index, 0);
  utils::submit_commands_to_queue(*m_cmd_pool, queue, [&](const CommandBuffer &cmd_buffer) {
      m_texture->image->set_layout(cmd_buffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  });
}

void Renderer::create_vertex_buffer()
{
  auto buffer_size = sizeof(vertices[0]) * vertices.size();
  m_vertex_buffer = utils::create_vertex_buffer(*m_device, buffer_size);
  m_vertex_buffer->upload(*m_cmd_pool, vertices.data(), buffer_size);
}

void Renderer::create_index_buffer()
{
  auto buffer_size = sizeof(indices[0]) * indices.size();
  m_index_buffer = utils::create_index_buffer(*m_device, buffer_size);
  m_index_buffer->upload(*m_cmd_pool, indices.data(), buffer_size);
}

void Renderer::update_uniform_buffer()
{
  static auto startTime = std::chrono::high_resolution_clock::now();

  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

  UniformMatrix ubo{};
  ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.proj = glm::perspective(glm::radians(45.0f), m_extent.width / (float) m_extent.height, 0.1f, 10.0f);
  ubo.proj[1][1] *= -1;

  m_uniform_buffers[m_render_context->get_active_frame_index()].upload(&ubo, sizeof(UniformMatrix));
}

bool Renderer::resize() {
  m_device->wait_idle();

  glfwGetFramebufferSize((GLFWwindow *)m_window->get_handle(),
                         (int *)&m_extent.width, (int *)&m_extent.height);
  while (m_extent.width == 0 || m_extent.height == 0) {
    glfwGetFramebufferSize((GLFWwindow *)m_window->get_handle(),
                           (int *)&m_extent.width, (int *)&m_extent.height);
    glfwWaitEvents();
  }

  m_framebuffers.clear();

  m_render_context->update(m_extent);

  create_framebuffer();

  return true;
}

void Renderer::render_image()
{
  auto &frame = m_render_context->get_active_frame();
  auto &queue = m_device->get_queue(m_queue_family_index, 0);
  auto &cmd_buffer = frame.request_command_buffer(queue);

  auto record_func = [&](const CommandBuffer &cmd_buffer) -> void {
      // record command buffer
    // render pass begin
    VkRenderPassBeginInfo render_pass_bi{};
    render_pass_bi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_bi.renderPass = m_render_pass->get_handle();
    render_pass_bi.framebuffer = m_framebuffers[m_render_context->get_active_frame_index()].get_handle();
    render_pass_bi.renderArea.offset = {0, 0};
    render_pass_bi.renderArea.extent = m_extent;

    std::array<VkClearValue, 2> clear_values{};
    clear_values[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
    clear_values[1].color = {1.0f, 0};

    render_pass_bi.clearValueCount = clear_values.size();
    render_pass_bi.pClearValues = clear_values.data();

    cmd_buffer.begin_render_pass(render_pass_bi, VK_SUBPASS_CONTENTS_INLINE);

    cmd_buffer.bind_pipeline(*m_graphic_pipeline);

    cmd_buffer.bind_descriptor_set(
        VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout->get_handle(),
        m_descriptor_sets[m_render_context->get_active_frame_index()].get_handle());

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

    cmd_buffer.bind_vertex_buffer(0, *m_vertex_buffer->buffer, 0);

    cmd_buffer.bind_index_buffer(*m_index_buffer->buffer, 0, VK_INDEX_TYPE_UINT16);

    cmd_buffer.draw_indexed(indices.size(), 1, 0, 0, 0);

    cmd_buffer.end_render_pass();

  };

  m_render_context->render(cmd_buffer, record_func);
}
