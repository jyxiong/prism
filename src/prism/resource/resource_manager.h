#pragma once

#include "prism/resource/resource.h"
#include <cstddef>
#include <concepts>
#include <memory>
#include <string>
#include <utility>
#include <typeindex>
#include <unordered_map>

namespace prism {

class ResourceManager;

template<ResourceType T>
class ResourceHandle {
public:
  ResourceHandle() : m_manager(nullptr) {}
  ResourceHandle(const std::string& id, ResourceManager* manager)
      : m_id(id), m_manager(manager) {}

  T* operator->() const {
    return get();
  }

  T& operator*() const {
    return *get();
  }

  operator bool() const {
    return isValid();
  }

  T* get() const;

  bool isValid() const;

private:
  std::string m_id;
  ResourceManager* m_manager;
};

class ResourceManager {
public:
  ResourceManager() = default;
  ~ResourceManager() {
    unloadAll();
  }

  template<ResourceType T>
  T* getResource(const std::string& id) {
    auto typeIt = m_resources.find(std::type_index(typeid(T)));
    if (typeIt == m_resources.end()) {
      return nullptr;
    }

    auto resourceIt = typeIt->second.find(id);
    if (resourceIt == typeIt->second.end()) {
      return nullptr;
    }

    return static_cast<T*>(resourceIt->second.get());
  }

  template<ResourceType T>
  bool hasResource(const std::string& id) const {
    auto typeIt = m_resources.find(std::type_index(typeid(T)));
    if (typeIt == m_resources.end()) {
      return false;
    }

    return typeIt->second.contains(id);
  }

  template<ResourceType T, typename... Args>
    requires std::constructible_from<T, const std::string&, Args...>
  ResourceHandle<T> load(const std::string& id, Args&&... args) {
    auto typeIndex = std::type_index(typeid(T));
    auto& typeResources = m_resources[typeIndex];
    auto resourceIt = typeResources.find(id);

    if (resourceIt != typeResources.end()) {
      ++m_refCounts[typeIndex][id].refCount;
      return ResourceHandle<T>(id, this);
    }

    auto resource = std::make_shared<T>(id, std::forward<Args>(args)...);
    if (!resource->load()) {
      return ResourceHandle<T>();
    }

    typeResources.emplace(id, resource);
    m_refCounts[typeIndex].emplace(id, ResourceData{1, resource});
    return ResourceHandle<T>(id, this);
  }

  template<ResourceType T>
  bool release(const std::string& id) {
    auto typeIndex = std::type_index(typeid(T));
    auto refTypeIt = m_refCounts.find(typeIndex);
    if (refTypeIt == m_refCounts.end()) {
      return false;
    }

    auto refIt = refTypeIt->second.find(id);
    if (refIt == refTypeIt->second.end()) {
      return false;
    }

    if (refIt->second.refCount > 1) {
      --refIt->second.refCount;
      return true;
    }

    if (!refIt->second.resource->unload()) {
      return false;
    }

    refTypeIt->second.erase(refIt);
    if (refTypeIt->second.empty()) {
      m_refCounts.erase(refTypeIt);
    }

    auto resourceTypeIt = m_resources.find(typeIndex);
    if (resourceTypeIt != m_resources.end()) {
      resourceTypeIt->second.erase(id);
      if (resourceTypeIt->second.empty()) {
        m_resources.erase(resourceTypeIt);
      }
    }

    return true;
  }

  template<ResourceType T>
  bool unloadAll() {
    auto typeIndex = std::type_index(typeid(T));
    auto resourceTypeIt = m_resources.find(typeIndex);
    if (resourceTypeIt == m_resources.end()) {
      return true;
    }

    bool success = true;
    for (auto& [id, resource] : resourceTypeIt->second) {
      success = resource->unload() && success;
    }

    m_resources.erase(typeIndex);
    m_refCounts.erase(typeIndex);
    return success;
  }

  bool unloadAll() {
    bool success = true;
    for (auto& [type, typeResources] : m_resources) {
      for (auto& [id, resource] : typeResources) {
        success = resource->unload() && success;
      }
    }

    m_resources.clear();
    m_refCounts.clear();
    return success;
  }

private:
  std::unordered_map<std::type_index,
                     std::unordered_map<std::string, std::shared_ptr<Resource>>>
      m_resources;

  struct ResourceData {
    std::size_t refCount;
    std::shared_ptr<Resource> resource;
  };
  std::unordered_map<std::type_index,
                     std::unordered_map<std::string, ResourceData>>
      m_refCounts;
};

/// Implementation of ResourceHandle<T>::get()
template<ResourceType T>
T* ResourceHandle<T>::get() const {
  if (m_manager) {
    return m_manager->getResource<T>(m_id);
  }
  return nullptr;
}

/// Implementation of ResourceHandle<T>::isValid()
template<ResourceType T>
bool ResourceHandle<T>::isValid() const {
  return m_manager && m_manager->hasResource<T>(m_id);
}

} // namespace prism