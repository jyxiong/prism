#pragma once

#include "prism/vulkan/device.h"
#include "prism/vulkan/image.h"

namespace prism
{
  class ImageViewCreateInfo : public VkImageViewCreateInfo
  {
    public:
      ImageViewCreateInfo();

      ImageViewCreateInfo& set_flags(VkImageViewCreateFlags flags);

      ImageViewCreateInfo& set_view_type(VkImageViewType view_type);

      ImageViewCreateInfo& set_format(VkFormat format);

      ImageViewCreateInfo& set_components(VkComponentMapping components);

      ImageViewCreateInfo& set_aspect_mask(VkImageAspectFlags aspect_mask);

      ImageViewCreateInfo& set_base_mip_level(uint32_t base_mip_level);

      ImageViewCreateInfo& set_level_count(uint32_t level_count);

      ImageViewCreateInfo& set_base_array_layer(uint32_t base_array_layer);

      ImageViewCreateInfo& set_layer_count(uint32_t layer_count);
  };

  class ImageView
  {
  public:
    ImageView(const Image &image, const ImageViewCreateInfo& info);

    ImageView(const ImageView &) = delete;

    ImageView(ImageView &&other) noexcept;

    ~ImageView();

    ImageView &operator=(const ImageView &) = delete;

    ImageView &operator=(ImageView &&) = delete;

    const VkImageView &get_handle() const;

  private:
    const Device &m_device;

    const Image &m_image;

    VkImageView m_handle{};

    ImageViewCreateInfo m_info{};
  };
} // namespace prism