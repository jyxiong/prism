#include "prism/vulkan/acceleration_structure_geometry.h"

using namespace prism;

AccelerationStructureGeometry::AccelerationStructureGeometry(const Triangles &triangles, VkGeometryFlagsKHR flags)
{
  m_handle.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
  m_handle.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
  m_handle.flags = flags;
  m_handle.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
  m_handle.geometry.triangles.vertexFormat = triangles.vertex_format;
  m_handle.geometry.triangles.vertexData.deviceAddress = triangles.vertex_data;
  m_handle.geometry.triangles.vertexStride = triangles.vertex_stride;
  m_handle.geometry.triangles.maxVertex = triangles.vertex_count;
  m_handle.geometry.triangles.indexData.deviceAddress = triangles.index_data;
  m_handle.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
  m_handle.geometry.triangles.transformData.deviceAddress = triangles.transform_data;

  m_primitive_count = triangles.vertex_count / 3;
}

AccelerationStructureGeometry::AccelerationStructureGeometry(const Aabbs &aabbs, VkGeometryFlagsKHR flags)
{
  m_handle.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
  m_handle.geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
  m_handle.flags = flags;
  m_handle.geometry.aabbs.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
  m_handle.geometry.aabbs.data.deviceAddress = aabbs.data;
  m_handle.geometry.aabbs.stride = aabbs.stride;

  m_primitive_count = aabbs.stride / sizeof(VkAabbPositionsKHR);
}

AccelerationStructureGeometry::AccelerationStructureGeometry(const Instances &instances, VkGeometryFlagsKHR flags)
{
  m_handle.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
  m_handle.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
  m_handle.flags = flags;
  m_handle.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
  m_handle.geometry.instances.data.deviceAddress = instances.data;
  m_handle.geometry.instances.arrayOfPointers = instances.array_of_pointers;

  m_primitive_count = 1;
}

const VkAccelerationStructureGeometryKHR &AccelerationStructureGeometry::get_handle() const
{
  return m_handle;
}

uint32_t AccelerationStructureGeometry::get_primitive_count() const
{
  return m_primitive_count;
}

uint32_t AccelerationStructureGeometry::get_transform_offset() const
{
  return m_transform_offset;
}