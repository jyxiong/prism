#pragma once

#include "prism/vulkan/device.h"
#include "prism/vulkan/descriptor_set_layout.h"
#include "prism/vulkan/descriptor_pool.h"

namespace prism
{
  class DescriptorSet
  {
  public:
    DescriptorSet(const Device &device, const DescriptorSetLayout& layout, const DescriptorPool& pool);

    DescriptorSet(const DescriptorSet &) = delete;

    DescriptorSet(DescriptorSet &&other) noexcept;

    ~DescriptorSet();

    DescriptorSet& operator=(const DescriptorSet &) = delete;

    DescriptorSet& operator=(DescriptorSet &&) = delete;

    void update(const std::vector<VkDescriptorBufferInfo> &buffer_infos);

    const VkDescriptorSet& get_handle() const;

  private:
    VkDescriptorSet m_handle;

    const Device& m_device;

    const DescriptorPool& m_pool;
    const DescriptorSetLayout& m_layout;
  };
}