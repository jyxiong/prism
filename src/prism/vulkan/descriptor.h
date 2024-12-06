#pragma once

namespace prism
{
  struct BufferInfo : public VkDescriptorBufferInfo
  {
    BufferInfo() = default;
    BufferInfo &set_buffer(VkBuffer buffer);
    BufferInfo &set_offset(VkDeviceSize offset);
    BufferInfo &set_range(VkDeviceSize range);
  };

  struct ImageInfo : public VkDescriptorImageInfo
  {
    ImageInfo() = default;
    ImageInfo &set_image_view(VkImageView image_view);
    ImageInfo &set_sampler(VkSampler sampler);
    ImageInfo &set_image_layout(VkImageLayout image_layout);
  };

  struct Descriptor
  {
    VkDescriptorType type;
    uint32_t binding;
    uint32_t array_element;
  };

  struct DescriptorBuffer : public Descriptor
  {
    BufferInfo info;
  };

  struct DescriptorImage : public Descriptor
  {
    ImageInfo info;
  };
}