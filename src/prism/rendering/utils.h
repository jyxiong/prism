#pragma once

#include "prism/rendering/buffer_data.h"
#include "prism/rendering/image_data.h"


namespace prism::utils {

  std::unique_ptr<BufferData> create_uniform_buffer(const Device &device, VkDeviceSize size);

  std::unique_ptr<BufferData> create_vertex_buffer(const Device &device, VkDeviceSize size);

  std::unique_ptr<BufferData> create_index_buffer(const Device &device, VkDeviceSize size);

  std::unique_ptr<ImageData> create_color_image_data(const Device &device, const VkExtent2D &extent, VkFormat format);

  std::unique_ptr<ImageData> create_depth_image_data(const Device &device, const VkExtent2D &extent);

  std::unique_ptr<ImageData> create_texture_image_data(const Device &device, const VkExtent2D &extent, VkFormat format);
}