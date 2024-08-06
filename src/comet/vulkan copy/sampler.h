#pragma once

#include "vulkan/vulkan.h"

#include "comet/vulkan/device.h"

namespace comet
{
  class SamplerCreateInfo : public VkSamplerCreateInfo
  {
  public:
    SamplerCreateInfo();

    SamplerCreateInfo &set_flags(VkSamplerCreateFlags flags);

    SamplerCreateInfo &set_mag_filter(VkFilter mag_filter);

    SamplerCreateInfo &set_min_filter(VkFilter min_filter);

    SamplerCreateInfo &set_mipmap_mode(VkSamplerMipmapMode mipmap_mode);

    SamplerCreateInfo &set_address_mode_u(VkSamplerAddressMode address_mode_u);

    SamplerCreateInfo &set_address_mode_v(VkSamplerAddressMode address_mode_v);

    SamplerCreateInfo &set_address_mode_w(VkSamplerAddressMode address_mode_w);

    SamplerCreateInfo &set_mip_lod_bias(float mip_lod_bias);

    SamplerCreateInfo &set_anisotropy_enable(VkBool32 anisotropy_enable);

    SamplerCreateInfo &set_max_anisotropy(float max_anisotropy);

    SamplerCreateInfo &set_compare_enable(VkBool32 compare_enable);

    SamplerCreateInfo &set_compare_op(VkCompareOp compare_op);

    SamplerCreateInfo &set_min_lod(float min_lod);

    SamplerCreateInfo &set_max_lod(float max_lod);

    SamplerCreateInfo &set_border_color(VkBorderColor border_color);

    SamplerCreateInfo &set_unnormalized_coordinates(VkBool32 unnormalized_coordinates);
  };

  class Sampler
  {
  public:
    Sampler(const Device &device, const SamplerCreateInfo &create_info);
    
    ~Sampler();

    VkSampler get_handle() const;

  private:
    VkSampler m_handle;

    const Device &m_device;

    SamplerCreateInfo m_info;
  };
}