#pragma once

#include "prism/vulkan/device.h"

namespace prism
{
  class Buffer;
  class Image;
  class CommandPool;
  class ComputePipeline;
  class GraphicsPipeline;

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

    void bind_pipeline(const ComputePipeline &pipeline) const;

    void bind_pipeline(const GraphicsPipeline &pipeline) const;

    void bind_descriptor_set(const VkPipelineBindPoint bind_point, const VkPipelineLayout layout, const VkDescriptorSet descriptor_set) const;

    void dispatch(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z) const;

    void begin_render_pass(const VkRenderPassBeginInfo &begin_info, VkSubpassContents contents) const;

    void end_render_pass() const;

    void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) const;

    void draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance) const;

    void set_viewport(const VkViewport &viewport) const;

    void set_scissor(const VkRect2D &scissor) const;

    void bind_vertex_buffer(uint32_t binding, const Buffer &buffer, VkDeviceSize offset) const;

    void bind_index_buffer(const Buffer &buffer, VkDeviceSize offset, VkIndexType index_type) const;

  private:
    VkCommandBuffer m_handle;

    const Device &m_device;
    const CommandPool &m_cmd_pool;

  }; // class CommandBuffer

} // namespace prism