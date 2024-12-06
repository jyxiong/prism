#include "prism/rendering/image_data.h"

#include "prism/rendering/buffer_data.h"
#include "prism/vulkan/command_buffer.h"
#include "prism/vulkan/command_pool.h"
#include "prism/vulkan/utils.h"

using namespace prism;

ImageData::ImageData(const Device &device, const ImageCreateInfo &create_info,
                     VkMemoryPropertyFlags memory_property_flags,
                     const ImageViewCreateInfo &view_create_info) {
  image = std::make_unique<Image>(device, create_info);
  device_memory = std::make_unique<DeviceMemory>(
      device, image->get_memory_requirements(), memory_property_flags);
  image->bind_memory(*device_memory, 0);

  image_view = std::make_unique<ImageView>(*image, view_create_info);
}

ImageData::~ImageData() {
  device_memory.reset();
  image.reset();
  image_view.reset();
}

void ImageData::upload(const CommandPool& cmd_pool, const void* data, const
VkDeviceSize size, VkImageAspectFlags aspect)
{
  const auto& device = image->get_device();

  auto stage_buffer = BufferData(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  stage_buffer.upload(data, size);

  auto queue_family_index =
  device.get_physical_device().get_queue_family_index(VK_QUEUE_GRAPHICS_BIT);
  auto &queue = device.get_queue(queue_family_index, 0);

  utils::submit_commands_to_queue(cmd_pool, queue, [&](const CommandBuffer
  &cmd_buffer) {
    // set layout to transfer destination
    VkImageSubresourceRange subresource_range{};
    subresource_range.aspectMask = aspect;
    subresource_range.baseMipLevel = 0;
    subresource_range.levelCount = image->get_mip_level_count();
    subresource_range.baseArrayLayer = 0;
    subresource_range.layerCount = image->get_array_layer_count();
    image->set_layout(cmd_buffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    subresource_range);

    VkBufferImageCopy region{};
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.layerCount = 1;
    region.imageExtent.width = image->get_extent().width;
    region.imageExtent.height = image->get_extent().height;
    region.imageExtent.depth = 1;
    cmd_buffer.copy_buffer_to_image(*stage_buffer.buffer, *image, {region});

    // set layout to shader read only optimal
    image->set_layout(cmd_buffer, VK_IMAGE_LAYOUT_GENERAL,
    subresource_range);

  });
}

Texture::Texture(const Device &device, const VkExtent2D &extent,
                 VkFormat format) {
  ImageCreateInfo create_info{};
  create_info.set_image_type(VK_IMAGE_TYPE_2D)
      .set_format(format)
      .set_extent({extent.width, extent.height, 1})
      .set_usage(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT)
      .set_initial_layout(VK_IMAGE_LAYOUT_UNDEFINED);

  image = std::make_unique<Image>(device, create_info);

  device_memory =
      std::make_unique<DeviceMemory>(device, image->get_memory_requirements(),
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  image->bind_memory(*device_memory, 0);

  ImageViewCreateInfo view_create_info{};
  view_create_info.set_view_type(VK_IMAGE_VIEW_TYPE_2D)
      .set_format(format)
      .set_aspect_mask(VK_IMAGE_ASPECT_COLOR_BIT);
  image_view = std::make_unique<ImageView>(*image, view_create_info);

  SamplerCreateInfo sampler_ci{};
  sampler_ci.set_mag_filter(VK_FILTER_LINEAR)
      .set_min_filter(VK_FILTER_LINEAR)
      .set_address_mode_u(VK_SAMPLER_ADDRESS_MODE_REPEAT)
      .set_address_mode_v(VK_SAMPLER_ADDRESS_MODE_REPEAT)
      .set_address_mode_w(VK_SAMPLER_ADDRESS_MODE_REPEAT)
      .set_anisotropy_enable(VK_FALSE)
      .set_max_anisotropy(1.0f)
      .set_border_color(VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK)
      .set_unnormalized_coordinates(VK_FALSE);
  sampler = std::make_unique<Sampler>(device, sampler_ci);
}

Texture::~Texture()
{
  sampler.reset();
  device_memory.reset();
  image.reset();
  image_view.reset();
}

void Texture::upload(const CommandPool &cmd_pool, const void *data,
                     const VkDeviceSize size, VkImageAspectFlags aspect) {
  const auto &device = image->get_device();

  auto stage_buffer = BufferData(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  stage_buffer.upload(data, size);

  auto queue_family_index = device.get_physical_device().get_queue_family_index(
      VK_QUEUE_GRAPHICS_BIT);
  auto &queue = device.get_queue(queue_family_index, 0);

  utils::submit_commands_to_queue(
      cmd_pool, queue, [&](const CommandBuffer &cmd_buffer) {
        image->set_layout(cmd_buffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          aspect);

        VkBufferImageCopy region{};
        region.imageSubresource.aspectMask = aspect;
        region.imageSubresource.layerCount = image->get_array_layer_count();
        region.imageExtent.width = image->get_extent().width;
        region.imageExtent.height = image->get_extent().height;
        region.imageExtent.depth = 1;
        cmd_buffer.copy_buffer_to_image(*stage_buffer.buffer, *image, {region});

        image->set_layout(cmd_buffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                          aspect);
      });
}

ColorAttachment::ColorAttachment(const Device &device, const VkExtent2D &extent, VkFormat format)
{
  ImageCreateInfo create_info{};
  create_info.set_image_type(VK_IMAGE_TYPE_2D)
      .set_format(format)
      .set_extent({extent.width, extent.height, 1})
      .set_usage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
      .set_initial_layout(VK_IMAGE_LAYOUT_UNDEFINED);

  image = std::make_unique<Image>(device, create_info);

  device_memory =
      std::make_unique<DeviceMemory>(device, image->get_memory_requirements(),
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  image->bind_memory(*device_memory, 0);

  ImageViewCreateInfo view_create_info{};
  view_create_info.set_view_type(VK_IMAGE_VIEW_TYPE_2D)
      .set_format(format)
      .set_aspect_mask(VK_IMAGE_ASPECT_COLOR_BIT);
  image_view = std::make_unique<ImageView>(*image, view_create_info);
}

ColorAttachment::~ColorAttachment()
{
  image_view.reset();
  device_memory.reset();
  image.reset();
}

DepthAttachment::DepthAttachment(const Device &device, const VkExtent2D &extent, VkFormat format)
  : format(format)
{
  ImageCreateInfo create_info{};
  create_info.set_image_type(VK_IMAGE_TYPE_2D)
      .set_format(format)
      .set_extent({extent.width, extent.height, 1})
      .set_usage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
      .set_initial_layout(VK_IMAGE_LAYOUT_UNDEFINED);

  image = std::make_unique<Image>(device, create_info);

  device_memory =
      std::make_unique<DeviceMemory>(device, image->get_memory_requirements(),
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  image->bind_memory(*device_memory, 0);

  ImageViewCreateInfo view_create_info{};
  view_create_info.set_view_type(VK_IMAGE_VIEW_TYPE_2D)
      .set_format(format)
      .set_aspect_mask(VK_IMAGE_ASPECT_DEPTH_BIT);
  image_view = std::make_unique<ImageView>(*image, view_create_info);
}

DepthAttachment::~DepthAttachment()
{
  image_view.reset();
  device_memory.reset();
  image.reset();
}