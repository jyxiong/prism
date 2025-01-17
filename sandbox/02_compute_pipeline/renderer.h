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
#include "prism/vulkan/instance.h"
#include "prism/vulkan/pipeline_layout.h"
#include "prism/vulkan/semaphore.h"
#include "prism/vulkan/surface.h"
#include "prism/vulkan/swapchain.h"


using namespace prism;

class Renderer {

public:
  Renderer();

  void render();

private:
  void create_window();
  void create_instance();
  void create_device();
  void create_surface();
  void create_swapchain();
  void create_render_target();
  void create_command_pool();
  void create_command_buffer();
  void create_pipeline();
  void create_descriptors();
  void create_sync_object();
  void draw();
  void recreate_swapchain();

private:
  std::unique_ptr<Window> m_window;

  std::unique_ptr<Instance> m_instance;
  std::unique_ptr<Device> m_device;

  std::unique_ptr<Surface> m_surface;

  std::unique_ptr<Swapchain> m_swapchain;
  uint32_t m_current_frame = 0;

  uint32_t m_queue_family_index;

  std::unique_ptr<ImageData> m_storage_data;

  std::unique_ptr<CommandPool> m_command_pool;
  std::vector<CommandBuffer> m_command_buffers;

  std::unique_ptr<DescriptorSetLayout> m_descriptor_set_layout;
  std::unique_ptr<PipelineLayout> m_pipeline_layout;
  std::unique_ptr<ComputePipeline> m_compute_pipeline;

  std::unique_ptr<DescriptorPool> m_descriptor_pool;
  std::unique_ptr<DescriptorSet> m_descriptor_set;

  std::vector<Semaphore> m_image_availabel_semaphores;
  std::vector<Semaphore> m_render_finished_semaphores;
  std::vector<Fence> m_in_flight_fences;
};