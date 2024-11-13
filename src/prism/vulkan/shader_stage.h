#pragma once

#include "prism/vulkan/shader_module.h"

namespace prism
{

  class ShaderStage : public VkPipelineShaderStageCreateInfo
  {
  public:
    ShaderStage();

    ShaderStage &set_flags(VkPipelineShaderStageCreateFlags flags);
    
    ShaderStage &set_stage(VkShaderStageFlagBits stage);
    
    ShaderStage &set_module(const ShaderModule &module);
    
    ShaderStage &set_entry_point(const std::string &entryPoint);
    
    ShaderStage &set_specialization_info(const VkSpecializationInfo &specializationInfo);
  };

} // namespace prism