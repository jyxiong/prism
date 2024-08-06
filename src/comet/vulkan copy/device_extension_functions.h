#pragma once

#include "vulkan/vulkan.h"

namespace comet
{
  class Device;

  class DeviceExtensionFunctions
  {
  public:
    DeviceExtensionFunctions(Device *instance);

    PFN_vkCreateAccelerationStructureKHR create_acceleration_structure = nullptr;
    PFN_vkDestroyAccelerationStructureKHR destroy_acceleration_structure = nullptr;
    PFN_vkGetAccelerationStructureDeviceAddressKHR get_acceleration_structure_device_address = nullptr;
    PFN_vkGetAccelerationStructureBuildSizesKHR get_acceleration_structure_build_sizes = nullptr;
    PFN_vkGetRayTracingShaderGroupHandlesKHR get_ray_tracing_shader_group_handles = nullptr;
    PFN_vkCmdBuildAccelerationStructuresKHR cmd_build_acceleration_structures = nullptr;
    PFN_vkCmdCopyAccelerationStructureKHR cmd_copy_acceleration_structure = nullptr;
    PFN_vkCmdWriteAccelerationStructuresPropertiesKHR cmd_write_acceleration_structures_properties = nullptr;
    PFN_vkCmdTraceRaysKHR cmd_trace_rays = nullptr;

    PFN_vkGetBufferDeviceAddressKHR get_buffer_device_address = nullptr;
  };

} // namespace comet