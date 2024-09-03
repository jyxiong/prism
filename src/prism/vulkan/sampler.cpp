#include "prism/vulkan/sampler.h"

#include "prism/vulkan/error.h"

using namespace prism;

SamplerCreateInfo::SamplerCreateInfo()
{
  sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  pNext = nullptr;
  flags = 0;
  magFilter = VK_FILTER_LINEAR;
  minFilter = VK_FILTER_LINEAR;
  mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  mipLodBias = 0.0f;
  anisotropyEnable = VK_FALSE;
  maxAnisotropy = 1.0f;
  compareEnable = VK_FALSE;
  compareOp = VK_COMPARE_OP_ALWAYS;
  minLod = 0.0f;
  maxLod = 0.0f;
  borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
  unnormalizedCoordinates = VK_FALSE;
}

SamplerCreateInfo& SamplerCreateInfo::set_flags(VkSamplerCreateFlags flags)
{
  this->flags = flags;
  return *this;
}

SamplerCreateInfo& SamplerCreateInfo::set_mag_filter(VkFilter mag_filter)
{
  this->magFilter = mag_filter;
  return *this;
}

SamplerCreateInfo& SamplerCreateInfo::set_min_filter(VkFilter min_filter)
{
  this->minFilter = min_filter;
  return *this;
}

SamplerCreateInfo& SamplerCreateInfo::set_mipmap_mode(VkSamplerMipmapMode mipmap_mode)
{
  this->mipmapMode = mipmap_mode;
  return *this;
}

SamplerCreateInfo& SamplerCreateInfo::set_address_mode_u(VkSamplerAddressMode address_mode_u)
{
  this->addressModeU = address_mode_u;
  return *this;
}

SamplerCreateInfo& SamplerCreateInfo::set_address_mode_v(VkSamplerAddressMode address_mode_v)
{
  this->addressModeV = address_mode_v;
  return *this;
}

SamplerCreateInfo& SamplerCreateInfo::set_address_mode_w(VkSamplerAddressMode address_mode_w)
{
  this->addressModeW = address_mode_w;
  return *this;
}

SamplerCreateInfo& SamplerCreateInfo::set_mip_lod_bias(float mip_lod_bias)
{
  this->mipLodBias = mip_lod_bias;
  return *this;
}

SamplerCreateInfo& SamplerCreateInfo::set_anisotropy_enable(VkBool32 anisotropy_enable)
{
  this->anisotropyEnable = anisotropy_enable;
  return *this;
}

SamplerCreateInfo& SamplerCreateInfo::set_max_anisotropy(float max_anisotropy)
{
  this->maxAnisotropy = max_anisotropy;
  return *this;
}

SamplerCreateInfo& SamplerCreateInfo::set_compare_enable(VkBool32 compare_enable)
{
  this->compareEnable = compare_enable;
  return *this;
}

SamplerCreateInfo& SamplerCreateInfo::set_compare_op(VkCompareOp compare_op)
{
  this->compareOp = compare_op;
  return *this;
}

SamplerCreateInfo& SamplerCreateInfo::set_min_lod(float min_lod)
{
  this->minLod = min_lod;
  return *this;
}

SamplerCreateInfo& SamplerCreateInfo::set_max_lod(float max_lod)
{
  this->maxLod = max_lod;
  return *this;
}

SamplerCreateInfo& SamplerCreateInfo::set_border_color(VkBorderColor border_color)
{
  this->borderColor = border_color;
  return *this;
}

SamplerCreateInfo& SamplerCreateInfo::set_unnormalized_coordinates(VkBool32 unnormalized_coordinates)
{
  this->unnormalizedCoordinates = unnormalized_coordinates;
  return *this;
}

Sampler::Sampler(const Device &device, const SamplerCreateInfo &create_info)
  : m_device(device), m_info(create_info)
{
  VK_CHECK(vkCreateSampler(m_device.get_handle(), &m_info, nullptr, &m_handle));
}

Sampler::~Sampler()
{
  if (m_handle != VK_NULL_HANDLE)
  {
    vkDestroySampler(m_device.get_handle(), m_handle, nullptr);
  }
}

VkSampler Sampler::get_handle() const
{
  return m_handle;
}