#include "prism/vulkan/device.h"
#include "prism/vulkan/command_pool.h"
#include "prism/vulkan/queue.h"
#include "prism/vulkan/command_buffer.h"
#include "prism/vulkan/buffer.h"

namespace prism::utils
{
  void submit_commands_to_queue(const CommandPool &cmd_pool, const Queue &queue, const std::function<void(const CommandBuffer &cmd_buffer)> &func)
  {
    auto cmd_buffer = std::make_unique<CommandBuffer>(cmd_pool);

    cmd_buffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    func(*cmd_buffer);
    cmd_buffer->end();

    queue.submit(*cmd_buffer);
    queue.wait_idle();
  }
}
