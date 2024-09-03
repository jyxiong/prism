#pragma once

#include "volk.h"

#include "comet/vulkan/physical_device.h"
#include "comet/vulkan/instance_extension_functions.h"

namespace comet
{
  class Instance
  {
  public:
    using ExtensionNames = std::vector<std::string>;
    using LayerNames = std::vector<std::string>;

  public:
    Instance(const ExtensionNames &extensions, const LayerNames& layers);

    ~Instance();

    VkInstance get_handle() const;

    const PhysicalDevice& pick_physical_device() const;

  private:
    bool check_extension_support(const std::vector<std::string> &layers);
    bool check_layer_support(const std::vector<std::string> &extensions);
    void query_physical_devices();
    void create_debug_messenger();

  private:
    VkInstance m_handle{VK_NULL_HANDLE};

    std::vector<const char *> m_layers{};

    std::vector<const char *> m_extensions{};

    std::vector<std::unique_ptr<PhysicalDevice>> m_physical_devices{};

    VkDebugUtilsMessengerEXT m_debug_utils_messenger{VK_NULL_HANDLE};

    std::unique_ptr<InstanceExtensionFunctions> m_extension_functions{nullptr};
  };
} // namespace comet