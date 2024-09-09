#pragma once

#include "prism/vulkan/physical_device.h"
#include "prism/vulkan/queue.h"
#include "prism/vulkan/device_features.h"
#include "prism/vulkan/device_extension_functions.h"

namespace prism
{
  class Device
  {
  public:
    using ExtensionNames = std::vector<std::string>;

  public:
    Device(const PhysicalDevice &physical_device, const ExtensionNames &extensions, const DeviceFeatures &features);
    ~Device();

    VkDevice get_handle() const;

    const PhysicalDevice &get_physical_device() const;

    const Queue &get_queue(uint32_t family_index, uint32_t index) const;

    const DeviceExtensionFunctions &get_extension_functions() const;

    bool check_extension_enable(const std::string &extension) const;

    void wait_idle() const;

  private:
    bool check_extension_support(const std::vector<std::string> &extensions);

  private:
    const PhysicalDevice &m_physical_device;

    VkDevice m_handle;

    std::vector<const char *> m_extensions;

    std::vector<std::vector<Queue>> m_queues;

    std::unique_ptr<DeviceExtensionFunctions> m_extension_functions{nullptr};
  };
}