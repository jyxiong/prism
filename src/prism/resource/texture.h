#pragma once

#include <string>

#include <volk.h>

#include "prism/resource/resource.h"


namespace prism {
class TextureResource : public Resource {
public:
  TextureResource(const std::string& id);
  ~TextureResource() override;

  int getWidth() const;
  int getHeight() const;
  int getChannels() const;

  VkImage getImage() const;
  VkDeviceMemory getImageMemory() const;
  VkDeviceSize getOffset() const;
  VkImageView getImageView() const;
  VkSampler getSampler() const;

protected:
  bool doLoad() override;
  bool doUnload() override;

private:
  bool loadImageData(const std::string& filePath, int& width, int& height,
                     int& channels);
  bool createVulkanImage(int width, int height, int channels);
  void reset();

  int m_width{0};
  int m_height{0};
  int m_channels{0};

  VkImage m_image{VK_NULL_HANDLE};
  VkDeviceMemory m_imageMemory{VK_NULL_HANDLE};
  VkDeviceSize m_offset{0};
  VkImageView m_imageView{VK_NULL_HANDLE};
  VkSampler m_sampler{VK_NULL_HANDLE};
};
} // namespace prism