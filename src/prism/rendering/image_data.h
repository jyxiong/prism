#pragma once

#include "prism/vulkan/device.h"
#include "prism/vulkan/device_memory.h"
#include "prism/vulkan/image.h"
#include "prism/vulkan/image_view.h"
#include "prism/vulkan/sampler.h"

namespace prism {
struct ImageData {
  std::unique_ptr<Image> image;
  std::unique_ptr<DeviceMemory> device_memory;
  std::unique_ptr<ImageView> image_view;

  ImageData(const Device &device, const ImageCreateInfo &create_info,
            VkMemoryPropertyFlags memory_property_flags,
            const ImageViewCreateInfo &view_create_info);

  ~ImageData();

  ImageData(const ImageData &) = delete;

  ImageData &operator=(const ImageData &) = delete;

  ImageData(ImageData &&) = default;

  ImageData &operator=(ImageData &&) = default;

  void upload(const CommandPool &cmd_pool, const void *data,
              const VkDeviceSize size,
              VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);

}; // struct ImageData

struct Texture {
  std::unique_ptr<Image> image;
  std::unique_ptr<DeviceMemory> device_memory;
  std::unique_ptr<ImageView> image_view;
  std::unique_ptr<Sampler> sampler;

  Texture(const Device &device, const VkExtent2D &extent, VkFormat format);

  ~Texture();

  Texture(const Texture &) = delete;

  Texture &operator=(const Texture &) = delete;

  Texture(Texture &&) = default;

  Texture &operator=(Texture &&) = default;

  void upload(const CommandPool &cmd_pool, const void *data,
              const VkDeviceSize size,
              VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);

}; // struct Texture

struct ColorAttachment {
  std::unique_ptr<Image> image;
  std::unique_ptr<DeviceMemory> device_memory;
  std::unique_ptr<ImageView> image_view;

  ColorAttachment(const Device &device, const VkExtent2D &extent,
                  VkFormat format);

  ~ColorAttachment();

  ColorAttachment(const ColorAttachment &) = delete;

  ColorAttachment &operator=(const ColorAttachment &) = delete;

  ColorAttachment(ColorAttachment &&) = default;

  ColorAttachment &operator=(ColorAttachment &&) = default;

}; // struct ColorAttachment

struct DepthAttachment
{
  VkFormat format;
  std::unique_ptr<Image> image;
  std::unique_ptr<DeviceMemory> device_memory;
  std::unique_ptr<ImageView> image_view;

  DepthAttachment(const Device &device, const VkExtent2D &extent, VkFormat format);

  ~DepthAttachment();

  DepthAttachment(const DepthAttachment &) = delete;

  DepthAttachment &operator=(const DepthAttachment &) = delete;

  DepthAttachment(DepthAttachment &&) = default;

  DepthAttachment &operator=(DepthAttachment &&) = default;

}; // struct DepthAttachment

} // namespace prism