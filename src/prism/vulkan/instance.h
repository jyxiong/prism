#pragma once

#include "prism/vulkan/device_features.h"
#include "prism/vulkan/instance_extension_functions.h"

namespace prism
{
  class PhysicalDevice;

  class Instance
  {
  public:
    using ExtensionNames = std::vector<const char*>;
    using LayerNames = std::vector<const char*>;

  public:
    Instance(const ExtensionNames &extensions, const LayerNames& layers);

    Instance(const Instance &) = delete;

    Instance(Instance &&other) = delete;

    ~Instance();

    Instance &operator=(const Instance &) = delete;

    Instance &operator=(Instance &&) = delete;

    VkInstance get_handle() const;

    const PhysicalDevice& pick_physical_device() const;
    const PhysicalDevice& query_physical_device(const ExtensionNames &required_extensions,
                                               const DeviceFeatures &required_features) const;

  private:
    void query_physical_devices();
    void create_debug_messenger();

  private:
    VkInstance m_handle{VK_NULL_HANDLE};

    std::vector<std::unique_ptr<PhysicalDevice>> m_physical_devices{};

    VkDebugUtilsMessengerEXT m_debug_utils_messenger{VK_NULL_HANDLE};

    std::unique_ptr<InstanceExtensionFunctions> m_extension_functions{nullptr};
  };
} // namespace prism