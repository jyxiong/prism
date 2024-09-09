#pragma once

#include "prism/vulkan/device.h"

namespace prism
{

  class ShaderModule
  {
  public:
    ShaderModule(const Device &device, const std::string &filename, VkShaderStageFlagBits stage, const std::string &entry_point = "main");

    ShaderModule(const ShaderModule &) = delete;

    ShaderModule(ShaderModule &&other) noexcept;

    ~ShaderModule();

    ShaderModule &operator=(const ShaderModule &) = delete;

    ShaderModule &operator=(ShaderModule &&) = delete;

    VkShaderModule get_handle() const;

    VkShaderStageFlagBits get_stage() const;

    const std::string &get_entry_point() const;

  private:
    VkShaderModule m_handle;

    const Device& m_device;

    std::string m_entry_point;

    VkShaderStageFlagBits m_stage;
  };

} // namespace prism