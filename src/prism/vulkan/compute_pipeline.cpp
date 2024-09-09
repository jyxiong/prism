#include "prism/vulkan/compute_pipeline.h"

#include "prism/vulkan/error.h"

using namespace prism;

ComputePipeline::ComputePipeline(const Device &device, const PipelineLayout &pipeline_layout, const ShaderModule& shader_module)
    : m_device(device)
{
  VkPipelineShaderStageCreateInfo shader_stage_info = {};
  shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shader_stage_info.stage = shader_module.get_stage();
  shader_stage_info.module = shader_module.get_handle();
  shader_stage_info.pName = shader_module.get_entry_point().c_str();

  VkComputePipelineCreateInfo pipeline_info = {};
  pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  pipeline_info.layout = pipeline_layout.get_handle();
  pipeline_info.stage = shader_stage_info;

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