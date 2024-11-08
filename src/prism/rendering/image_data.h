#pragma once

#include "prism/vulkan/device.h"
#include "prism/vulkan/device_memory.h"
#include "prism/vulkan/image.h"
#include "prism/vulkan/image_view.h"

namespace prism 
{
    struct ImageData
    {
      std::unique_ptr<Image> image;
      std::unique_ptr<DeviceMemory> device_memory;
      std::unique_ptr<ImageView> image_view;

      ImageData(const Device& device, const ImageCreateInfo& create_info, VkMemoryPropertyFlags memory_property_flags, const ImageViewCreateInfo& view_create_info);
      
      ~ImageData();

      void upload(const CommandPool& cmd_pool, const void* data, const VkDeviceSize size, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);

    }; // struct ImageData

} // namespace prism