#include "prism/rendering/utils.h"

namespace prism::utils {

std::unique_ptr<BufferData> create_vertex_buffer(const Device &device,
                                                 VkDeviceSize size) {
  return std::make_unique<BufferData>(device, size,
                                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                          VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

std::unique_ptr<BufferData> create_index_buffer(const Device &device,
                                                VkDeviceSize size) {
  return std::make_unique<BufferData>(device, size,
                                      VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                          VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

std::unique_ptr<ImageData> create_color_image_data(const Device &device,
                                                   const VkExtent2D &extent,
                                                   VkFormat format) {
  ImageCreateInfo create_info{};
  create_info.set_image_type(VK_IMAGE_TYPE_2D)
      .set_format(format)
      .set_extent({extent.width, extent.height, 1})
      .set_usage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
      .set_initial_layout(VK_IMAGE_LAYOUT_UNDEFINED);

  ImageViewCreateInfo view_create_info{};
  view_create_info.set_view_type(VK_IMAGE_VIEW_TYPE_2D)
      .set_format(format)
      .set_aspect_mask(VK_IMAGE_ASPECT_COLOR_BIT);

  return std::make_unique<ImageData>(device, create_info,
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                     view_create_info);
}

std::unique_ptr<ImageData> create_depth_image_data(const Device &device,
                                                   const VkExtent2D &extent) {
  ImageCreateInfo create_info{};
  create_info.set_image_type(VK_IMAGE_TYPE_2D)
      .set_format(VK_FORMAT_D32_SFLOAT)
      .set_extent({extent.width, extent.height, 1})
      .set_usage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
      .set_initial_layout(VK_IMAGE_LAYOUT_UNDEFINED);

  ImageViewCreateInfo view_create_info{};
  view_create_info.set_view_type(VK_IMAGE_VIEW_TYPE_2D)
      .set_format(VK_FORMAT_D32_SFLOAT)
      .set_aspect_mask(VK_IMAGE_ASPECT_DEPTH_BIT);

  return std::make_unique<ImageData>(device, create_info,
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                     view_create_info);
}

std::unique_ptr<ImageData> create_texture_image_data(const Device &device, const VkExtent2D &extent, VkFormat format)
{
    ImageCreateInfo create_info{};
    create_info.set_image_type(VK_IMAGE_TYPE_2D)
        .set_format(format)
        .set_extent({extent.width, extent.height, 1})
        .set_usage(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        .set_initial_layout(VK_IMAGE_LAYOUT_UNDEFINED);
    
    ImageViewCreateInfo view_create_info{};
    view_create_info.set_view_type(VK_IMAGE_VIEW_TYPE_2D)
        .set_format(format)
        .set_aspect_mask(VK_IMAGE_ASPECT_COLOR_BIT);
    
    return std::make_unique<ImageData>(device, create_info,
                                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                         view_create_info);
}

} // namespace prism::utils
