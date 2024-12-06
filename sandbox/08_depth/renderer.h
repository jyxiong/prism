#pragma once

#include "prism/platform/window.h"
#include "prism/rendering/buffer_data.h"
#include "prism/rendering/image_data.h"
#include "prism/vulkan/command_buffer.h"
#include "prism/vulkan/command_pool.h"
#include "prism/vulkan/descriptor_pool.h"
#include "prism/vulkan/descriptor_set.h"
#include "prism/vulkan/descriptor_set_layout.h"
#include "prism/vulkan/device.h"
#include "prism/vulkan/graphics_pipeline.h"
#include "prism/vulkan/instance.h"
#include "prism/vulkan/pipeline_layout.h"
#include "prism/vulkan/render_pass.h"
#include "prism/vulkan/surface.h"
#include "prism/vulkan/swapchain.h"
#include "prism/vulkan/framebuffer.h"

#include "prism/rendering/render_context.h"


using namespace prism;

class Renderer {

public:
  Renderer();

  void render_loop();

private:
  void create_window();
  void create_instance();
  void create_device();
  void create_surface();
  void create_command_pool();

  void create_render_context();

  void create_descriptor_layout();
  void create_render_pass();
  void create_pipeline();
  void create_framebuffer();

  void create_texture();

  void create_uniform_buffer();
  void update_uniform_buffer();

  void create_descriptors();

  void create_vertex_buffer();
  void create_index_buffer();
  
  bool resize();

  void render_image();

private:
  VkExtent2D m_extent = {800, 800};

  std::unique_ptr<Window> m_window;

  std::unique_ptr<Instance> m_instance;

  std::unique_ptr<Device> m_device;

  std::unique_ptr<Surface> m_surface;

  std::unique_ptr<CommandPool> m_cmd_pool;

  std::unique_ptr<RenderContext> m_render_context;
  std::unique_ptr<DepthAttachment> m_depth_attachment;

  uint32_t m_queue_family_index;

  // input
  std::unique_ptr<BufferData> m_vertex_buffer;
  std::unique_ptr<BufferData> m_index_buffer;

  // parameter
  // uniform buffer ...
  std::unique_ptr<Texture> m_texture;
  std::vector<UniformBuffer> m_uniform_buffers;

  // pipeline parameter
  std::unique_ptr<DescriptorPool> m_descriptor_pool;
  std::vector<DescriptorSet> m_descriptor_sets;

  std::unique_ptr<RenderPass> m_render_pass;
  std::vector<Framebuffer> m_framebuffers;

  // pipeline
  std::unique_ptr<DescriptorSetLayout> m_descriptor_set_layout;
  std::unique_ptr<PipelineLayout> m_pipeline_layout;
  std::unique_ptr<GraphicsPipeline> m_graphic_pipeline;
};