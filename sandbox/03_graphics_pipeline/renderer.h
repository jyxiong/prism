#pragma once

#include "prism/platform/window.h"
#include "prism/rendering/image_data.h"
#include "prism/vulkan/command_buffer.h"
#include "prism/vulkan/command_pool.h"
#include "prism/vulkan/compute_pipeline.h"
#include "prism/vulkan/descriptor_pool.h"
#include "prism/vulkan/descriptor_set.h"
#include "prism/vulkan/descriptor_set_layout.h"
#include "prism/vulkan/device.h"
#include "prism/vulkan/fence.h"
#include "prism/vulkan/graphics_pipeline.h"
#include "prism/vulkan/instance.h"
#include "prism/vulkan/pipeline_layout.h"
#include "prism/vulkan/render_pass.h"
#include "prism/vulkan/semaphore.h"
#include "prism/vulkan/surface.h"
#include "prism/vulkan/swapchain.h"
#include "prism/vulkan/framebuffer.h"


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
  void create_swapchain();
  void create_command_pool();
  void create_command_buffer();
  void create_render_target();
  void create_render_pass();
  void create_pipeline();
  void create_framebuffer();
  void create_descriptors();
  void create_sync_object();
  void draw();
  void display();
  void recreate_swapchain();

private:
  VkExtent2D m_extent = {800, 600};

  std::unique_ptr<Window> m_window;

  std::unique_ptr<Instance> m_instance;
  std::unique_ptr<Device> m_device;

  std::unique_ptr<Surface> m_surface;

  std::unique_ptr<Swapchain> m_swapchain;
  // std::vector<Framebuffer> m_framebuffers;

  uint32_t m_current_frame = 0;

  uint32_t m_queue_family_index;

  std::unique_ptr<CommandPool> m_command_pool;
  std::vector<CommandBuffer> m_command_buffers;
  std::unique_ptr<CommandBuffer> m_draw_command_buffer;

  // output
  std::unique_ptr<ImageData> m_storage_data;

  // input
  // vertex buffer ...

  // parameter
  // uniform buffer ...

  // pipeline output
  std::unique_ptr<RenderPass> m_render_pass;
  std::unique_ptr<Framebuffer> m_framebuffer;

  // pipeline parameter
  std::unique_ptr<DescriptorPool> m_descriptor_pool;
  std::unique_ptr<DescriptorSet> m_descriptor_set;

  // pipeline
  std::unique_ptr<DescriptorSetLayout> m_descriptor_set_layout;
  std::unique_ptr<PipelineLayout> m_pipeline_layout;
  std::unique_ptr<GraphicsPipeline> m_graphic_pipeline;

  // sync object
  std::vector<Semaphore> m_image_availabel_semaphores;
  std::vector<Semaphore> m_render_finished_semaphores;
  std::vector<Fence> m_in_flight_fences;
};