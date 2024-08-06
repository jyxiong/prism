#include "comet/vulkan/acceleration_structure_instance.h"

using namespace comet;

AccelerationStructureInstance &AccelerationStructureInstance::transform(const glm::mat3x4 &transform)
{
  memcpy(&m_handle.transform, &transform, sizeof(transform));  
  return *this;
}

AccelerationStructureInstance &AccelerationStructureInstance::transform(const VkTransformMatrixKHR &transform)
{
  m_handle.transform = transform;
  return *this;
}

AccelerationStructureInstance &AccelerationStructureInstance::instance_custom_index(uint32_t custom_index)
{
  m_handle.instanceCustomIndex = custom_index;
  return *this;
}

AccelerationStructureInstance &AccelerationStructureInstance::instance_shader_binding_table_record_offset(uint32_t shader_binding_table_record_offset)
{
  m_handle.instanceShaderBindingTableRecordOffset = shader_binding_table_record_offset;
  return *this;
}

AccelerationStructureInstance &AccelerationStructureInstance::flags(VkGeometryInstanceFlagsKHR flags)
{
  m_handle.flags = flags;
  return *this;
}

AccelerationStructureInstance &AccelerationStructureInstance::mask(uint8_t mask)
{
  m_handle.mask = mask;
  return *this;
}

AccelerationStructureInstance &AccelerationStructureInstance::acceleration_structure(uint64_t acceleration_structure_handle)
{
  m_handle.accelerationStructureReference = acceleration_structure_handle;
  return *this;
}

const VkAccelerationStructureInstanceKHR &AccelerationStructureInstance::get_handle() const
{
  return m_handle;
}
