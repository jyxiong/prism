#include "prism/vulkan/shader_stage.h"

using namespace prism;

ShaderStage::ShaderStage()
{
  sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  pNext = nullptr;
  flags = 0;
  stage = VK_SHADER_STAGE_VERTEX_BIT;
  module = VK_NULL_HANDLE;
  pName = nullptr;
  pSpecializationInfo = nullptr;
}

ShaderStage &ShaderStage::set_flags(VkPipelineShaderStageCreateFlags flags)
{
  this->flags = flags;
  return *this;
}

ShaderStage &ShaderStage::set_stage(VkShaderStageFlagBits stage)
{
  this->stage = stage;
  return *this;
}

ShaderStage &ShaderStage::set_module(const ShaderModule &module)
{
  this->module = module.get_handle();
  return *this;
}

ShaderStage &ShaderStage::set_entry_point(const std::string &entryPoint)
{
  pName = entryPoint.c_str();
  return *this;
}

ShaderStage &ShaderStage::set_specialization_info(const VkSpecializationInfo &specializationInfo)
{
  pSpecializationInfo = &specializationInfo;
  return *this;
}
