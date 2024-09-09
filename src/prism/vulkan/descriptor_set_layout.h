#pragma once

#include "prism/vulkan/device.h"

namespace prism
{
  class DescriptorSetLayout
  {
  public:
    using Bindings = std::vector<VkDescriptorSetLayoutBinding>;

  public:
    DescriptorSetLayout(const Device &device, const Bindings &bindings);

    DescriptorSetLayout(const DescriptorSetLayout &) = delete;

    DescriptorSetLayout(DescriptorSetLayout &&other) noexcept;

    ~DescriptorSetLayout();

    DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

    DescriptorSetLayout &operator=(DescriptorSetLayout &&) = delete;

    const VkDescriptorSetLayout &get_handle() const;

    std::vector<VkDescriptorPoolSize> get_descriptor_pool_sizes() const;

  private:
    VkDescriptorSetLayout m_handle;

    const Device &m_device;

    Bindings m_bindings;
  };

}