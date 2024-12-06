#pragma once

#include "prism/vulkan/device.h"
#include "prism/vulkan/descriptor_set_layout.h"
#include "prism/vulkan/descriptor_pool.h"
#include "prism/vulkan/descriptor.h"

namespace prism
{
  class DescriptorSet
  {
  public:
    DescriptorSet(const Device &device, const DescriptorSetLayout& layout, const DescriptorPool& pool);

    ~DescriptorSet();

    DescriptorSet(const DescriptorSet &) = delete;

    DescriptorSet(DescriptorSet &&other) noexcept;

    DescriptorSet& operator=(const DescriptorSet &) = delete;

    DescriptorSet& operator=(DescriptorSet &&) = delete;

    const VkDescriptorSet& get_handle() const;

    void write(const std::vector<VkDescriptorBufferInfo> &buffer_infos);

    void write(const std::vector<DescriptorBuffer> &buffers, const std::vector<DescriptorImage> &images);

  private:
    VkDescriptorSet m_handle;

    const Device& m_device;

    const DescriptorPool& m_pool;
    const DescriptorSetLayout& m_layout;
  };
}