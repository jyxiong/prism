#include "prism/vulkan/command_buffer.h"

#include "prism/vulkan/buffer.h"
#include "prism/vulkan/command_pool.h"
#include "prism/vulkan/error.h"
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