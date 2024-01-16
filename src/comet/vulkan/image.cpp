#include "comet/vulkan/image.h"

using namespace comet;

Image::Image(Device const &device,
             VkImage handle,
             const VkExtent3D &extent,
             VkFormat format,
             VkImageUsageFlags image_usage,
             VkSampleCountFlagBits sample_count)
    : m_device{const_cast<Device *>(&device)},
      m_handle{handle},
      m_format{format},
      m_extent{extent}
{
    m_subresource.mipLevel = 1;
    m_subresource.arrayLayer = 1;
}

Device &Image::get_device() const
{
    return *m_device;
}

VkImage Image::get_handle() const
{
    return m_handle;
}

VkFormat Image::get_format() const
{
    return m_format;
}

VkImageSubresource Image::get_subresource() const
{
    return m_subresource;
}

std::unordered_set<ImageView *> &Image::get_views()
{
    return m_views;
}