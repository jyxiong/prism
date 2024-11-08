#pragma once

#include "prism/rendering/buffer_data.h"
#include "prism/vulkan/command_buffer.h"
#include "prism/vulkan/command_pool.h"
#include "prism/vulkan/device.h"
#include "prism/vulkan/instance.h"

using namespace prism;

class Renderer {

public:
  Renderer();

  void render();
  void save(const std::string& path);

private:
  void create_instance();
  void create_device();
  void create_framebuffer();
  void create_command_pool();
  void create_command_buffer();

private:
  std::unique_ptr<Instance> m_instance;
  std::unique_ptr<Device> m_device;

  uint32_t m_queue_family_index;

  std::unique_ptr<BufferData> m_framebuffer;

  std::unique_ptr<CommandPool> m_command_pool;
  std::unique_ptr<CommandBuffer> m_command_buffer;
};