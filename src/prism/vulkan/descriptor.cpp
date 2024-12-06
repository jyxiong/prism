#include "prism/vulkan/descriptor.h"

using namespace prism;

BufferInfo &BufferInfo::set_buffer(VkBuffer buffer)
{
  this->buffer = buffer;
  return *this;
}

BufferInfo &BufferInfo::set_offset(VkDeviceSize offset)
{
  this->offset = offset;
  return *this;
}

BufferInfo &BufferInfo::set_range(VkDeviceSize range)
{
  this->range = range;
  return *this;
}

ImageInfo &ImageInfo::set_image_view(VkImageView image_view)
{
  this->imageView = image_view;
  return *this;
}

ImageInfo &ImageInfo::set_sampler(VkSampler sampler)
{
  this->sampler = sampler;
  return *this;
}

ImageInfo &ImageInfo::set_image_layout(VkImageLayout image_layout)
{
  this->imageLayout = image_layout;
  return *this;
}