#pragma once

namespace prism
{
  class Device;
  class CommandPool;
  class Queue;
  class CommandBuffer;
  class Buffer;

  namespace utils
  {
    bool check_layers_support(const std::vector<const char *>& required_layers, const std::vector<VkLayerProperties>& available_layers);

    bool check_extensions_support(const std::vector<const char *>& required_extensions, const std::vector<VkExtensionProperties>& available_extensions);

    void submit_commands_to_queue(const CommandPool& cmd_pool, const Queue& queue, const std::function<void(const CommandBuffer&)>& func);

  } // namespace utils

} // namespace prism
