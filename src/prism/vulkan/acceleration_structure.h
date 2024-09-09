#pragma once

#include "prism/vulkan/device.h"
#include "prism/vulkan/buffer.h"
#include "prism/vulkan/device_memory.h"
#include "prism/vulkan/acceleration_structure_geometry.h"

namespace prism
{
  class AccelerationStructure
  {
  public:
    AccelerationStructure(const Device& device, VkAccelerationStructureTypeKHR type, const std::vector<AccelerationStructureGeometry> &geometries);

    ~AccelerationStructure();

    const VkAccelerationStructureKHR &get_handle() const;

    VkDeviceAddress get_device_address() const;

  private:
    VkAccelerationStructureKHR m_handle;

    const Device& m_device;

    std::unique_ptr<Buffer> m_buffer;
    std::unique_ptr<DeviceMemory> m_memory;

    VkDeviceAddress m_device_address;

    std::unique_ptr<Buffer> m_scratch_buffer;
    std::unique_ptr<DeviceMemory> m_scratch_memory;

  }; // class AccelerationStructure
} // namespace prism
