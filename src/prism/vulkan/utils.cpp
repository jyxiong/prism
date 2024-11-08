#include "prism/vulkan/utils.h"

#include <algorithm>

#include "prism/vulkan/command_pool.h"
#include "prism/vulkan/queue.h"
#include "prism/vulkan/command_buffer.h"

namespace prism::utils
{
  bool check_layers_support(const std::vector<const char*> &required_layers, const std::vector<VkLayerProperties> &available_layers)
  {
    return std::all_of(required_layers.begin(), required_layers.end(), [&](const char* required_layer)
                       { return std::any_of(available_layers.begin(), available_layers.end(), [&](const VkLayerProperties &layer)
                                           { return strcmp(layer.layerName, required_layer) == 0; }); });
  }

  bool check_extensions_support(const std::vector<const char*> &required_extensions, const std::vector<VkExtensionProperties> &available_extensions)
  {
    return std::all_of(required_extensions.begin(), required_extensions.end(), [&](const char* required_extension)
                       { return std::any_of(available_extensions.begin(), available_extensions.end(), [&](const VkExtensionProperties &extension)
                                           { return strcmp(extension.extensionName, required_extension) == 0; }); });
  }

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
