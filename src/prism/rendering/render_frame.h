#pragma once

#include "prism/vulkan/command_buffer.h"
#include "prism/vulkan/command_pool.h"
#include "prism/vulkan/descriptor_pool.h"
#include "prism/vulkan/descriptor_set.h"
#include "prism/vulkan/device.h"
#include "prism/vulkan/fence.h"
#include "prism/vulkan/semaphore.h"
#include "prism/vulkan/image_view.h"

namespace prism {
class RenderFrame {
public:
  RenderFrame(const Device &device, const Image& image);

  ~RenderFrame();

  RenderFrame(const RenderFrame &) = delete;

  RenderFrame(RenderFrame &&);

  RenderFrame &operator=(const RenderFrame &) = delete;

  RenderFrame &operator=(RenderFrame &&) = delete;

  const std::vector<ImageView> &get_image_views() const;

  const Semaphore &get_semaphore() const;

  Fence &get_fence() const;

  CommandBuffer &request_command_buffer(const Queue &queue);

  void reset();

private:
  CommandPool &get_command_pool(const Queue &queue);

private:
  const Device &m_device;

  std::vector<ImageView> m_image_views;

  std::map<uint32_t, std::unique_ptr<CommandPool>> m_cmd_pools;

  std::unique_ptr<DescriptorPool> m_descriptor_pool;
  std::unique_ptr<DescriptorSet> m_descriptor_set;

  std::unique_ptr<Semaphore> m_semaphore;
  std::unique_ptr<Fence> m_fence;

}; // class RenderFrame

} // namespace prism