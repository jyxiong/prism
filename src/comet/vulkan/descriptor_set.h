#pragma once

#include "comet/vulkan/device.h"
#include "comet/vulkan/descriptor_set_layout.h"
#include "comet/vulkan/descriptor_pool.h"

namespace comet
{
  class DescriptorSet
  {
  public:
    DescriptorSet(const Device &device, const DescriptorSetLayout& layout, const DescriptorPool& pool);
    ~DescriptorSet();

    void update(const std::vector<VkDescriptorBufferInfo> &buffer_infos);

    const VkDescriptorSet& get_handle() const;

  private:
    VkDescriptorSet m_handle;

    const Device& m_device;

    const DescriptorPool& m_pool;
    const DescriptorSetLayout& m_layout;
  };
}