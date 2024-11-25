#pragma once

#include "prism/vulkan/device.h"

namespace prism
{
  class CommandBuffer;

  class CommandPool
  {
  public:
    CommandPool(const Device &device, uint32_t queue_family_index, VkCommandPoolCreateFlags flags = 0);

    CommandPool(const CommandPool &) = delete;

    CommandPool(CommandPool &&other) noexcept;

    ~CommandPool();

    CommandPool& operator=(const CommandPool &) = delete;

    CommandPool& operator=(CommandPool &&) = delete;

    VkCommandPool get_handle() const;

    const Device &get_device() const;

    CommandBuffer &request_command_buffer(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    void reset();

    void clear_command_buffers();

    void reset_command_buffers();

  private:
    VkCommandPool m_handle;

    const Device& m_device;

    std::vector<std::unique_ptr<CommandBuffer>> m_cmd_buffers;

    uint32_t m_active_cmd_buffer_count{0};
  };

} // namespace prism