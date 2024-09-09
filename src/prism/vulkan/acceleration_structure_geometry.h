#pragma once

namespace prism
{
  class AccelerationStructureGeometry
  {
  public:
    struct Triangles
    {
      VkFormat vertex_format;
      VkDeviceAddress vertex_data;
      VkDeviceSize vertex_stride;
      uint32_t vertex_count;
      VkDeviceAddress index_data;
      VkDeviceAddress transform_data;
    };

    struct Aabbs
    {
      VkDeviceAddress data;
      VkDeviceSize stride;
    };

    struct Instances
    {
      VkDeviceAddress data;
      VkBool32 array_of_pointers;
    };

  public:
    AccelerationStructureGeometry(const Triangles &triagnles, VkGeometryFlagsKHR flags = VK_GEOMETRY_OPAQUE_BIT_KHR);

    AccelerationStructureGeometry(const Aabbs &aabbs, VkGeometryFlagsKHR flags = VK_GEOMETRY_OPAQUE_BIT_KHR);

    AccelerationStructureGeometry(const Instances &instances, VkGeometryFlagsKHR flags = VK_GEOMETRY_OPAQUE_BIT_KHR);

    const VkAccelerationStructureGeometryKHR &get_handle() const;

    uint32_t get_primitive_count() const;

    uint32_t get_transform_offset() const;

  private:
    VkAccelerationStructureGeometryKHR m_handle{};

    uint32_t m_primitive_count{};
    uint32_t m_transform_offset{};

  }; // class AccelerationStructureGeometry
} // namespace prism