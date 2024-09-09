#include "prism/vulkan/image_view.h"

#include "prism/vulkan/error.h"

using namespace prism;

VkImageAspectFlags aspect_mask(VkFormat format)
{
  if (format == VK_FORMAT_D16_UNORM_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D32_SFLOAT_S8_UINT)
  {
    return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
  }
  else if (format == VK_FORMAT_D16_UNORM || format == VK_FORMAT_D32_SFLOAT || format == VK_FORMAT_X8_D24_UNORM_PACK32)
  {
    return VK_IMAGE_ASPECT_DEPTH_BIT;
  }
  else
  {
    return VK_IMAGE_ASPECT_COLOR_BIT;
  }
}

VkImageViewType image_view_type(VkImageType image_type)
{
  switch (image_type)
  {
  case VK_IMAGE_TYPE_1D:
    return VK_IMAGE_VIEW_TYPE_1D;
  case VK_IMAGE_TYPE_2D:
    return VK_IMAGE_VIEW_TYPE_2D;
  case VK_IMAGE_TYPE_3D:
    return VK_IMAGE_VIEW_TYPE_3D;
  default:
    return VK_IMAGE_VIEW_TYPE_2D;
  }
}

ImageViewCreateInfo::ImageViewCreateInfo()
{
  sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  pNext = nullptr;
  flags = 0;
  viewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
  format = VK_FORMAT_UNDEFINED;
  components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceRange.baseMipLevel = 0;
  subresourceRange.levelCount = 1;
  subresourceRange.baseArrayLayer = 0;
  subresourceRange.layerCount = 1;
}

ImageViewCreateInfo &ImageViewCreateInfo::set_flags(VkImageViewCreateFlags flags)
{
  this->flags = flags;
  return *this;
}

ImageViewCreateInfo &ImageViewCreateInfo::set_view_type(VkImageViewType view_type)
{
  viewType = view_type;
  return *this;
}

ImageViewCreateInfo &ImageViewCreateInfo::set_format(VkFormat format)
{
  this->format = format;
  return *this;
}

ImageViewCreateInfo &ImageViewCreateInfo::set_components(VkComponentMapping components)
{
  this->components = components;
  return *this;
}

ImageViewCreateInfo &ImageViewCreateInfo::set_aspect_mask(VkImageAspectFlags aspect_mask)
{
  subresourceRange.aspectMask = aspect_mask;
  return *this;
}

ImageViewCreateInfo &ImageViewCreateInfo::set_base_mip_level(uint32_t base_mip_level)
{
  subresourceRange.baseMipLevel = base_mip_level;
  return *this;
}

ImageViewCreateInfo &ImageViewCreateInfo::set_level_count(uint32_t level_count)
{
  subresourceRange.levelCount = level_count;
  return *this;
}

ImageViewCreateInfo &ImageViewCreateInfo::set_base_array_layer(uint32_t base_array_layer)
{
  subresourceRange.baseArrayLayer = base_array_layer;
  return *this;
}

ImageViewCreateInfo &ImageViewCreateInfo::set_layer_count(uint32_t layer_count)
{
  subresourceRange.layerCount = layer_count;
  return *this;
}

ImageView::ImageView(const Image &image, const ImageViewCreateInfo &info)
    : m_device{image.get_device()}, m_image{image}, m_info(info)
{
  if (m_info.viewType == VK_IMAGE_VIEW_TYPE_MAX_ENUM)
  {
    m_info.viewType = image_view_type(image.get_image_type());
  }

  if (m_info.format == VK_FORMAT_UNDEFINED)
  {
    m_info.format = m_image.get_format();
  }

  if (m_info.subresourceRange.levelCount == 0)
  {
    m_info.subresourceRange.levelCount = m_image.get_mip_level_count();
  }

  if (m_info.subresourceRange.layerCount == 0)
  {
    m_info.subresourceRange.layerCount = m_image.get_array_layer_count();
  }

  m_info.subresourceRange.aspectMask = aspect_mask(m_info.format);

  m_info.image = m_image.get_handle();

  VK_CHECK(vkCreateImageView(m_device.get_handle(), &m_info, nullptr, &m_handle));
}

ImageView::ImageView(ImageView &&other) noexcept
    : m_handle(std::exchange(other.m_handle, VK_NULL_HANDLE)),
      m_device(other.m_device),
      m_image(other.m_image),
      m_info(other.m_info)
{
}

ImageView::~ImageView()
{
  if (m_handle != VK_NULL_HANDLE)
  {
    vkDestroyImageView(m_device.get_handle(), m_handle, nullptr);
  }
}

const VkImageView &ImageView::get_handle() const
{
  return m_handle;
}
