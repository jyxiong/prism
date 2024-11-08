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
    using ExtensionNames = std::vector<const char*>;

  public:
    Device(const PhysicalDevice &physical_device, const ExtensionNames &extensions, const DeviceFeatures &features);

    Device(const Device &) = delete;

    Device(Device &&other) = delete;

    ~Device();

    Device &operator=(const Device &) = delete;

    Device &operator=(Device &&) = delete;

    VkDevice get_handle() const;

    const PhysicalDevice &get_physical_device() const;

    const Queue &get_queue(uint32_t family_index, uint32_t index) const;

    const DeviceExtensionFunctions &get_extension_functions() const;

    void wait_idle() const;

  private:
    const PhysicalDevice &m_physical_device;

    VkDevice m_handle;

    std::vector<std::vector<Queue>> m_queues;

    std::unique_ptr<DeviceExtensionFunctions> m_extension_functions{nullptr};
  };
}