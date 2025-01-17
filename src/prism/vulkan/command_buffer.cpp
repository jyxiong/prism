#include "prism/vulkan/command_buffer.h"

#include "prism/vulkan/buffer.h"
#include "prism/vulkan/command_pool.h"
#include "prism/vulkan/compute_pipeline.h"
#include "prism/vulkan/graphics_pipeline.h"
#include "prism/vulkan/image.h"


using namespace prism;

CommandBuffer::CommandBuffer(const CommandPool &cmd_pool,
                             VkCommandBufferLevel level)
    : m_device(cmd_pool.get_device()), m_cmd_pool(cmd_pool) {
  VkCommandBufferAllocateInfo allocate_info = {};
  allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocate_info.commandPool = m_cmd_pool.get_handle();
  allocate_info.level = level;
  allocate_info.commandBufferCount = 1;

  VK_CHECK(vkAllocateCommandBuffers(m_device.get_handle(), &allocate_info,
                                    &m_handle));
}

CommandBuffer::CommandBuffer(CommandBuffer &&other) noexcept
    : m_handle(std::exchange(other.m_handle, VK_NULL_HANDLE)),
      m_device(other.m_device), m_cmd_pool(other.m_cmd_pool) {}

CommandBuffer::~CommandBuffer() {
  if (m_handle != VK_NULL_HANDLE) {
    vkFreeCommandBuffers(m_device.get_handle(), m_cmd_pool.get_handle(), 1,
                         &m_handle);
  }
}

const VkCommandBuffer &CommandBuffer::get_handle() const { return m_handle; }

void CommandBuffer::begin(VkCommandBufferUsageFlags flags) const {
  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = flags;

  VK_CHECK(vkBeginCommandBuffer(m_handle, &begin_info));
}

void CommandBuffer::end() const { VK_CHECK(vkEndCommandBuffer(m_handle)); }

void CommandBuffer::reset() const {
  VK_CHECK(vkResetCommandBuffer(m_handle, 0));
}

void CommandBuffer::copy_buffer(
    const Buffer &src, const Buffer &dst,
    const std::vector<VkBufferCopy> &regions) const {
  vkCmdCopyBuffer(m_handle, src.get_handle(), dst.get_handle(), regions.size(),
                  regions.data());
}

void CommandBuffer::copy_buffer_to_image(
    const Buffer &src, const Image &dst,
    const std::vector<VkBufferImageCopy> &regions) const {
  VkBufferImageCopy region = {};

  vkCmdCopyBufferToImage(m_handle, src.get_handle(), dst.get_handle(),
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, regions.size(),
                         regions.data());
}

void CommandBuffer::copy_image(const Image &src, const Image &dst,
                               const std::vector<VkImageCopy> &regions) const {
  vkCmdCopyImage(m_handle, src.get_handle(),
                 VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst.get_handle(),
                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, regions.size(),
                 regions.data());
}

void CommandBuffer::fill_buffer(const Buffer &buffer, VkDeviceSize offset,
                                VkDeviceSize size, uint32_t data) const {
  vkCmdFillBuffer(m_handle, buffer.get_handle(), offset, size, data);
}

void CommandBuffer::pipeline_barrier(
    VkPipelineStageFlags src_stage, VkPipelineStageFlags dst_stage,
    VkDependencyFlags dependency_flags,
    const std::vector<VkMemoryBarrier> &memory_barriers,
    const std::vector<VkBufferMemoryBarrier> &buffer_memory_barriers,
    const std::vector<VkImageMemoryBarrier> &image_memory_barriers) const {
  vkCmdPipelineBarrier(
      m_handle, src_stage, dst_stage, dependency_flags, memory_barriers.size(),
      memory_barriers.data(), buffer_memory_barriers.size(),
      buffer_memory_barriers.data(), image_memory_barriers.size(),
      image_memory_barriers.data());
}

void CommandBuffer::bind_pipeline(const ComputePipeline &pipeline) const {
  vkCmdBindPipeline(m_handle, VK_PIPELINE_BIND_POINT_COMPUTE,
                    pipeline.get_handle());
}

void CommandBuffer::bind_pipeline(const GraphicsPipeline &pipeline) const {
  vkCmdBindPipeline(m_handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline.get_handle());
}

void CommandBuffer::bind_descriptor_set(const VkPipelineBindPoint bind_point,
                                        const VkPipelineLayout layout,
                                        const VkDescriptorSet descriptor_set) const {
  vkCmdBindDescriptorSets(m_handle, bind_point, layout, 0, 1, &descriptor_set,
                          0, nullptr);
}

void CommandBuffer::dispatch(uint32_t group_count_x, uint32_t group_count_y,
                             uint32_t group_count_z) const {
  vkCmdDispatch(m_handle, group_count_x, group_count_y, group_count_z);
}

void CommandBuffer::begin_render_pass(const VkRenderPassBeginInfo &begin_info,
                                      VkSubpassContents contents) const {
  vkCmdBeginRenderPass(m_handle, &begin_info, contents);
}

void CommandBuffer::end_render_pass() const { vkCmdEndRenderPass(m_handle); }

void CommandBuffer::draw(uint32_t vertex_count, uint32_t instance_count,
                         uint32_t first_vertex, uint32_t first_instance) const {
  vkCmdDraw(m_handle, vertex_count, instance_count, first_vertex,
            first_instance);
}

void CommandBuffer::draw_indexed(uint32_t index_count, uint32_t instance_count,
                                 uint32_t first_index, int32_t vertex_offset,
                                 uint32_t first_instance) const {
  vkCmdDrawIndexed(m_handle, index_count, instance_count, first_index,
                   vertex_offset, first_instance);
}

void CommandBuffer::set_viewport(const VkViewport &viewport) const {
  vkCmdSetViewport(m_handle, 0, 1, &viewport);
}

void CommandBuffer::set_scissor(const VkRect2D &scissor) const {
  vkCmdSetScissor(m_handle, 0, 1, &scissor);
}

void CommandBuffer::bind_vertex_buffer(uint32_t binding, const Buffer &buffer,
                                       VkDeviceSize offset) const {
  auto vk_buffer = buffer.get_handle();
  vkCmdBindVertexBuffers(m_handle, binding, 1, &vk_buffer, &offset);
}

void CommandBuffer::bind_index_buffer(const Buffer &buffer, VkDeviceSize offset,
                                      VkIndexType index_type) const {
  vkCmdBindIndexBuffer(m_handle, buffer.get_handle(), offset, index_type);
}