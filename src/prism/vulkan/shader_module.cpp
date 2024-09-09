#include "prism/vulkan/shader_module.h"

#include "prism/core/filesystem.h"
#include "prism/vulkan/error.h"

using namespace prism;

ShaderModule::ShaderModule(const Device &device, const std::string &filename, VkShaderStageFlagBits stage, const std::string &entry_point)
    : m_device(device), m_stage(stage), m_entry_point(entry_point)
{
  auto code = read_file(filename, true);
  VkShaderModuleCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size();
  create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

  VK_CHECK(vkCreateShaderModule(m_device.get_handle(), &create_info, nullptr, &m_handle));
}

ShaderModule::ShaderModule(ShaderModule &&other) noexcept
    : m_handle(std::exchange(other.m_handle, VK_NULL_HANDLE)),
      m_device(other.m_device),
      m_stage(other.m_stage),
      m_entry_point(std::move(other.m_entry_point))
{
}

ShaderModule::~ShaderModule()
{
  if (m_handle != VK_NULL_HANDLE)
  {
    vkDestroyShaderModule(m_device.get_handle(), m_handle, nullptr);
  }
}

VkShaderModule ShaderModule::get_handle() const
{
  return m_handle;
}

VkShaderStageFlagBits ShaderModule::get_stage() const
{
  return m_stage;
}

const std::string &ShaderModule::get_entry_point() const
{
  return m_entry_point;
}