#pragma once

#include "prism/vulkan/device.h"

namespace prism
{
  class DescriptorPool
  {
  public:
    DescriptorPool(const Device &device, const std::vector<VkDescriptorPoolSize> &pool_sizes, uint32_t max_sets);
    ~DescriptorPool();

    VkDescriptorSet allocate(const VkDescriptorSetLayout &layout);

    void free(const VkDescriptorSet &descriptor_set);

    VkDescriptorPool get_handle() const;

  private:
    VkDescriptorPool m_handle;

    const Device& m_device;

    uint32_t m_max_sets;

    std::vector<VkDescriptorPoolSize> m_sizes;
  };
}