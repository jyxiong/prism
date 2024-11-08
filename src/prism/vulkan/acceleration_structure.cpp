#include "prism/vulkan/acceleration_structure.h"

#include "prism/vulkan/error.h"
#include "prism/vulkan/utils.h"
#include "prism/vulkan/command_pool.h"
#include "prism/vulkan/command_buffer.h"

using namespace prism;

AccelerationStructure::AccelerationStructure(const Device &device, VkAccelerationStructureTypeKHR type, const std::vector<AccelerationStructureGeometry> &geometries)
    : m_device(device)
{
  auto geometry_count = geometries.size();
  std::vector<VkAccelerationStructureGeometryKHR> geometry_data(geometry_count);
  std::vector<uint32_t> primitive_counts(geometry_count);
  std::vector<VkAccelerationStructureBuildRangeInfoKHR> build_range_infos(geometry_count);
  for (size_t i = 0; i < geometry_count; ++i)
  {
    geometry_data[i] = geometries[i].get_handle();
    primitive_counts[i] = geometries[i].get_primitive_count();

    build_range_infos[i].primitiveCount = primitive_counts[i];
    build_range_infos[i].primitiveOffset = 0;
    build_range_infos[i].firstVertex = 0;
    build_range_infos[i].transformOffset = geometries[i].get_transform_offset();
  }

  VkAccelerationStructureBuildGeometryInfoKHR build_geometry_info{};
  build_geometry_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
  build_geometry_info.type = type;
  build_geometry_info.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
  build_geometry_info.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
  build_geometry_info.geometryCount = static_cast<uint32_t>(geometry_data.size());
  build_geometry_info.pGeometries = geometry_data.data();

  VkAccelerationStructureBuildSizesInfoKHR build_sizes_info = {};
  build_sizes_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
  m_device.get_extension_functions().get_acceleration_structure_build_sizes(
      device.get_handle(),
      VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
      &build_geometry_info,
      primitive_counts.data(),
      &build_sizes_info);

  m_buffer = std::make_unique<Buffer>(
      device,
      build_sizes_info.accelerationStructureSize,
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
  m_memory = std::make_unique<DeviceMemory>(device, m_buffer->get_memory_requirements(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT);
  m_buffer->bind_memory(*m_memory);

  VkAccelerationStructureCreateInfoKHR create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
  create_info.type = type;
  create_info.buffer = m_buffer->get_handle();
  create_info.size = build_sizes_info.accelerationStructureSize;

  VK_CHECK(m_device.get_extension_functions().create_acceleration_structure(m_device.get_handle(), &create_info, nullptr, &m_handle));

  VkAccelerationStructureDeviceAddressInfoKHR device_address_info = {};
  device_address_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
  device_address_info.accelerationStructure = m_handle;
  m_device_address = m_device.get_extension_functions().get_acceleration_structure_device_address(m_device.get_handle(), &device_address_info);

  auto queue_family_index = m_device.get_physical_device().get_queue_family_index(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT);
  const auto &queue = m_device.get_queue(queue_family_index, 0);
  CommandPool command_pool(m_device, queue_family_index);
  utils::submit_commands_to_queue(
      command_pool,
      queue,
      [&](const CommandBuffer &cmd_buffer)
      {
        m_scratch_buffer = std::make_unique<Buffer>(
            device,
            build_sizes_info.buildScratchSize,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
        m_scratch_memory = std::make_unique<DeviceMemory>(device, m_scratch_buffer->get_memory_requirements(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT);
        m_scratch_buffer->bind_memory(*m_scratch_memory);

        build_geometry_info.dstAccelerationStructure = m_handle;
        build_geometry_info.scratchData.deviceAddress = m_scratch_buffer->get_device_address();

        auto build_range_info_data = &*build_range_infos.data();
        m_device.get_extension_functions().cmd_build_acceleration_structures(
            cmd_buffer.get_handle(),
            1,
            &build_geometry_info,
            &build_range_info_data);
      });
}

AccelerationStructure::~AccelerationStructure()
{
  if (m_handle != VK_NULL_HANDLE)
  {
    m_device.get_extension_functions().destroy_acceleration_structure(m_device.get_handle(), m_handle, nullptr);
  }
}

const VkAccelerationStructureKHR &AccelerationStructure::get_handle() const
{
  return m_handle;
}

VkDeviceAddress AccelerationStructure::get_device_address() const
{
  return m_device_address;
}