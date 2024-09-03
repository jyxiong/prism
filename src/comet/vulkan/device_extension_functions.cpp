#include "comet/vulkan/device_extension_functions.h"

#include "comet/vulkan/device.h"

using namespace comet;

DeviceExtensionFunctions::DeviceExtensionFunctions(Device* device)
{
    create_acceleration_structure = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(vkGetDeviceProcAddr(device->get_handle(), "vkCreateAccelerationStructureKHR"));
    destroy_acceleration_structure = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(vkGetDeviceProcAddr(device->get_handle(), "vkDestroyAccelerationStructureKHR"));
    get_acceleration_structure_device_address = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(vkGetDeviceProcAddr(device->get_handle(), "vkGetAccelerationStructureDeviceAddressKHR"));
    get_acceleration_structure_build_sizes = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(vkGetDeviceProcAddr(device->get_handle(), "vkGetAccelerationStructureBuildSizesKHR"));
    get_ray_tracing_shader_group_handles = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(vkGetDeviceProcAddr(device->get_handle(), "vkGetRayTracingShaderGroupHandlesKHR"));
    cmd_build_acceleration_structures = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(device->get_handle(), "vkCmdBuildAccelerationStructuresKHR"));
    cmd_copy_acceleration_structure = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureKHR>(vkGetDeviceProcAddr(device->get_handle(), "vkCmdCopyAccelerationStructureKHR"));
    cmd_write_acceleration_structures_properties = reinterpret_cast<PFN_vkCmdWriteAccelerationStructuresPropertiesKHR>(vkGetDeviceProcAddr(device->get_handle(), "vkCmdWriteAccelerationStructuresPropertiesKHR"));
    cmd_trace_rays = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(vkGetDeviceProcAddr(device->get_handle(), "vkCmdTraceRaysKHR"));

    get_buffer_device_address = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(device->get_handle(), "vkGetBufferDeviceAddressKHR"));
  }