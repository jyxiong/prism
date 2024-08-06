#pragma once

#include <vector>

#include "comet/vulkan/device.h"

namespace comet
{
  class DescriptorSetLayout
  {
  public:
    using Bindings = std::vector<VkDescriptorSetLayoutBinding>;

  public:
    DescriptorSetLayout(const Device &device, const Bindings &bindings);
    ~DescriptorSetLayout();

    const VkDescriptorSetLayout &get_handle() const;

    std::vector<VkDescriptorPoolSize> get_descriptor_pool_sizes() const;

  private:
    VkDescriptorSetLayout m_handle;

    const Device &m_device;

    Bindings m_bindings;
  };

}