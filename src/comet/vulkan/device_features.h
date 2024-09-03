#pragma once

#include "volk.h"

namespace comet
{

  class DeviceFeatures
  {
  public:
    DeviceFeatures() = default;
    ~DeviceFeatures();

    DeviceFeatures(const DeviceFeatures &) = delete;
    DeviceFeatures &operator=(const DeviceFeatures &) = delete;

    template <typename T>
    T &get(VkStructureType type);

    template <typename T>
    void request(VkStructureType type, VkBool32 T::*member);

    void clear();
    void *data() const;

  private:
    struct FeatureHeader
    {
      VkStructureType sType;
      void *pNext;
    };

    std::map<VkStructureType, FeatureHeader *> m_features;
  };

  template <typename T>
  T &DeviceFeatures::get(VkStructureType type)
  {
    if (auto itr = m_features.find(type); itr != m_features.end())
      return *reinterpret_cast<T *>(itr->second);

    auto *feature = new T{};
    feature->sType = type;
    feature->pNext = nullptr;

    m_features[type] = reinterpret_cast<FeatureHeader *>(feature);

    if constexpr (std::is_same_v<T, VkPhysicalDeviceFeatures>)
    {
      return feature->features;
    }

    return *feature;
  }

  template <typename T>
  void DeviceFeatures::request(VkStructureType type, VkBool32 T::*member)
  {
    auto &feature = get<T>(type);
    feature.*member = VK_TRUE;
  }

}