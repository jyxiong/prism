#pragma once

#include "prism/vulkan/device.h"

namespace prism
{
  class Buffer;
  class Image;
  class CommandPool;

  class CommandBuffer
  {
  public:
    explicit CommandBuffer(const CommandPool &cmd_pool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    CommandBuffer(const CommandBuffer &) = delete;

    CommandBuffer(CommandBuffer &&other) noexcept;

    ~CommandBuffer();

    CommandBuffer& operator=(const CommandBuffer &) = delete;

    CommandBuffer& operator=(CommandBuffer &&) = delete;

    const VkCommandBuffer &get_handle() const;

    void begin(VkCommandBufferUsageFlags flags = 0) const;

    void end() const;

    void reset() const;

    void copy_buffer(const Buffer &src, const Buffer &dst, const std::vector<VkBufferCopy>& regions) const;

    void copy_buffer_to_image(const Buffer &src, const Image &dst, const std::vector<VkBufferImageCopy>& regions) const;

    void copy_image(const Image &src, const Image &dst, const std::vector<VkImageCopy>& regions) const;

    void fill_buffer(const Buffer &buffer, VkDeviceSize offset, VkDeviceSize size, uint32_t data) const;

    void pipeline_barrier(VkPipelineStageFlags src_stage, VkPipelineStageFlags dst_stage,
                          VkDependencyFlags dependency_flags,
                          const std::vector<VkMemoryBarrier> &memory_barriers,
                          const std::vector<VkBufferMemoryBarrier> &buffer_memory_barriers,
                          const std::vector<VkImageMemoryBarrier> &image_memory_barriers) const;

  private:
    VkCommandBuffer m_handle;

    const Device &m_device;
    const CommandPool &m_cmd_pool;

  }; // class CommandBuffer

} // namespace prism