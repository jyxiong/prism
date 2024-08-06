#include "comet/vulkan/device_features.h"

using namespace comet;

DeviceFeatures::~DeviceFeatures()
{
  clear();
}

void DeviceFeatures::clear()
{
  for (auto &feature : m_features)
  {
    delete feature.second;
  }

  m_features.clear();
}

void *DeviceFeatures::data() const
{
  if (m_features.empty())
    return nullptr;

  FeatureHeader *previous = nullptr;
  for (auto itr = m_features.rbegin(); itr != m_features.rend(); ++itr)
  {
    itr->second->pNext = previous;
    previous = itr->second;
  }

  return const_cast<void *>(reinterpret_cast<const void *>(m_features.begin()->second));
}