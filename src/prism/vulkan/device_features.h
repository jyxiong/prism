#pragma once

#include <functional>

namespace prism {

class PhysicalDevice;

class DeviceFeatures {
public:
  DeviceFeatures() = default;
  ~DeviceFeatures();

  DeviceFeatures(const DeviceFeatures &) = delete;
  DeviceFeatures &operator=(const DeviceFeatures &) = delete;

  template <typename FeatureStruct, VkStructureType stype>
  FeatureStruct &get() {
    if (auto itr = m_features.find(stype); itr != m_features.end())
      return *reinterpret_cast<FeatureStruct *>(itr->second.first);

    FeatureStruct *feature = new FeatureStruct{};
    feature->sType = stype;
    feature->pNext = nullptr;

    m_features[stype].first = reinterpret_cast<FeatureHeader *>(feature);
    m_features[stype].second = [](FeatureHeader *ptr) {
      delete reinterpret_cast<FeatureStruct *>(ptr);
    };

    return *feature;
  }

  template <typename FeatureStruct, VkStructureType stype>
  void request(VkBool32 FeatureStruct::*member) {
    auto &feature = get<FeatureStruct, stype>();
    feature.*member = VK_TRUE;

    // 存入一个独立的 validator lambda，校验时不需要知道具体类型
    m_validators.push_back([member](VkPhysicalDevice handle) -> bool {
      FeatureStruct supported{};
      supported.sType = stype;

      VkPhysicalDeviceFeatures2 features2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
      features2.pNext = &supported;
      vkGetPhysicalDeviceFeatures2(handle, &features2);

      return (supported.*member) == VK_TRUE;
    });
  }

  bool is_supported_by(const PhysicalDevice &physical_device) const;

  void clear();
  void *data() const;

private:
  struct FeatureHeader {
    VkStructureType sType;
    void *pNext;
  };

  using DeleteHandler = void (*)(FeatureHeader *);

  std::map<VkStructureType, std::pair<FeatureHeader *, DeleteHandler>> m_features;
  std::vector<std::function<bool(VkPhysicalDevice)>> m_validators;
};

} // namespace prism