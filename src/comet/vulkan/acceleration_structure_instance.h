#pragma once

#include "glm/glm.hpp"

#include "comet/vulkan/buffer.h"

namespace comet
{
  class AccelerationStructureInstance
  {
  public:
    AccelerationStructureInstance() = default;

    AccelerationStructureInstance& transform(const glm::mat3x4 &transform);

    AccelerationStructureInstance& transform(const VkTransformMatrixKHR &transform);

    AccelerationStructureInstance& instance_custom_index(uint32_t custom_index);

    AccelerationStructureInstance& instance_shader_binding_table_record_offset(uint32_t shader_binding_table_record_offset);

    AccelerationStructureInstance& flags(VkGeometryInstanceFlagsKHR flags);

    AccelerationStructureInstance& mask(uint8_t mask);

    AccelerationStructureInstance& acceleration_structure(uint64_t acceleration_structure_handle);

    const VkAccelerationStructureInstanceKHR &get_handle() const;

  private:
    VkAccelerationStructureInstanceKHR m_handle{};
  
  }; // class AccelerationStructureInstance
} // namespace comet