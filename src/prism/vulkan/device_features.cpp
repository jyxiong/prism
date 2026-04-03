#include "prism/vulkan/device_features.h"

#include "prism/vulkan/physical_device.h"

using namespace prism;

DeviceFeatures::~DeviceFeatures() { clear(); }

bool DeviceFeatures::is_supported_by(const PhysicalDevice &physical_device) const {
  for (const auto &validator : m_validators) {
    if (!validator(physical_device.get_handle()))
      return false;
  }
  return true;
}

void DeviceFeatures::clear() {
  for (auto &feature : m_features) {
    feature.second.second(feature.second.first);
  }
  m_features.clear();
  m_validators.clear();
}

void *DeviceFeatures::data() const {
  if (m_features.empty())
    return nullptr;

  FeatureHeader *previous = nullptr;
  for (auto itr = m_features.rbegin(); itr != m_features.rend(); ++itr) {
    itr->second.first->pNext = previous;
    previous = itr->second.first;
  }

  return const_cast<void *>(
      reinterpret_cast<const void *>(m_features.begin()->second.first));
}