#include "prism/vulkan/compute_pipeline.h"

#include "prism/vulkan/error.h"

using namespace prism;

ComputePipeline::ComputePipeline(const Device &device, const PipelineLayout &pipeline_layout, const ShaderStage& shader_stage)
    : m_device(device)
{
  VkComputePipelineCreateInfo pipeline_info = {};
  pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  pipeline_info.layout = pipeline_layout.get_handle();
  pipeline_info.stage = shader_stage;

  VK_CHECK(vkCreateComputePipelines(m_device.get_handle(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_handle));
}

ComputePipeline::ComputePipeline(ComputePipeline &&other) noexcept
    : m_handle(std::exchange(other.m_handle, VK_NULL_HANDLE)),
      m_device(other.m_device)
{
}

ComputePipeline::~ComputePipeline()
{
  if (m_handle != VK_NULL_HANDLE)
  {
    vkDestroyPipeline(m_device.get_handle(), m_handle, nullptr);
  }
}

const VkPipeline &ComputePipeline::get_handle() const
{
  return m_handle;
}