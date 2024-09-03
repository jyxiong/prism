#pragma once



#include "comet/vulkan/device.h"

namespace comet
{
  class CommandPool;

  class CommandBuffer
  {
  public:
    CommandBuffer(const CommandPool &cmd_pool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    
    ~CommandBuffer();

    const VkCommandBuffer &get_handle() const;

    void begin(VkCommandBufferUsageFlags flags = 0);

    void end();

    void reset();

  private:
    VkCommandBuffer m_handle;

    const Device &m_device;
    const CommandPool &m_cmd_pool;

  }; // class CommandBuffer

} // namespace comet