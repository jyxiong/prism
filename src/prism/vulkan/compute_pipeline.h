#pragma once

#include "prism/vulkan/device.h"
#include "prism/vulkan/shader_module.h"
#include "prism/vulkan/pipeline_layout.h"

namespace prism
{
  class ComputePipeline
  {
  public:
    ComputePipeline(const Device &device, const PipelineLayout &pipeline_layout, const ShaderModule& shader_module);

    ~ComputePipeline();

    const VkPipeline &get_handle() const;

  private:
    VkPipeline m_handle;

    const Device& m_device;

  }; // class ComputePipeline

} // namespace prism