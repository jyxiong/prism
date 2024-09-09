#pragma once

#include "prism/vulkan/device.h"
#include "prism/vulkan/descriptor_set_layout.h"

namespace prism
{
  class PipelineLayout
  {
  public:
    PipelineLayout(const Device &device, const DescriptorSetLayout &descriptor_set_layout, const std::vector<VkPushConstantRange> &push_constant_ranges = {});
    
    PipelineLayout(const Device &device, const std::vector<DescriptorSetLayout> &descriptor_set_layouts);

    PipelineLayout(const PipelineLayout &) = delete;

    PipelineLayout(PipelineLayout &&other) noexcept;
    
    ~PipelineLayout();

    PipelineLayout& operator=(const PipelineLayout &) = delete;

    PipelineLayout& operator=(PipelineLayout &&) = delete;

    const VkPipelineLayout &get_handle() const;

  private:
    VkPipelineLayout m_handle;

    const Device &m_device;

  }; // class PipelineLayout

} // namespace prism