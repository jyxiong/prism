#include "prism/resource/texture.h"

#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace prism;

TextureResource::TextureResource(const std::string& id) : Resource(id) {}

TextureResource::~TextureResource() {
  unload();
}

int TextureResource::getWidth() const {
  return m_width;
}

int TextureResource::getHeight() const {
  return m_height;
}

int TextureResource::getChannels() const {
  return m_channels;
}

VkImage TextureResource::getImage() const {
  return m_image;
}

VkDeviceMemory TextureResource::getImageMemory() const {
  return m_imageMemory;
}

VkDeviceSize TextureResource::getOffset() const {
  return m_offset;
}

VkImageView TextureResource::getImageView() const {
  return m_imageView;
}

VkSampler TextureResource::getSampler() const {
  return m_sampler;
}

bool TextureResource::doLoad() {
  std::string filePath = "textures/" + getId() + ".ktx";

  int width = 0;
  int height = 0;
  int channels = 0;
  if (!loadImageData(filePath, width, height, channels)) {
    return false;
  }

  if (!createVulkanImage(width, height, channels)) {
    reset();
    return false;
  }

  m_width = width;
  m_height = height;
  m_channels = channels;
  return true;
}

bool TextureResource::doUnload() {
  reset();
  return true;
}

bool TextureResource::loadImageData(const std::string& filePath, int& width, int& height,
                            int& channels) {
  auto *data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
  if (!data) {
  }
  return false;
}

bool TextureResource::createVulkanImage(int width, int height, int channels) {
  (void)width;
  (void)height;
  (void)channels;
  return false;
}

void TextureResource::reset() {
  m_width = 0;
  m_height = 0;
  m_channels = 0;
  m_image = VK_NULL_HANDLE;
  m_imageMemory = VK_NULL_HANDLE;
  m_offset = 0;
  m_imageView = VK_NULL_HANDLE;
  m_sampler = VK_NULL_HANDLE;
}