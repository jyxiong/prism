#pragma once

#include <functional>

namespace comet
{
  class Device;
  class CommandPool;
  class Queue;
  class CommandBuffer;
  class Buffer;

  namespace utils
  {
    void submit_commands_to_queue(const CommandPool& cmd_pool, const Queue& queue, const std::function<void(const CommandBuffer&)>& func);


  } // namespace utils

} // namespace comet
