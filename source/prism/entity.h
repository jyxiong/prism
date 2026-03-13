#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "prism/component.h"

namespace prism {
class Entity {
public:
  explicit Entity(const std::string &name) : m_name(name) {}

  virtual ~Entity() = default;

  void initialize();

  void update(std::chrono::microseconds deltaTime);

  void render();

  void setName(const std::string &name) { m_name = name; }
  const std::string &getName() const { return m_name; }

  void setActive(bool active) { m_active = active; }
  bool getActive() const { return m_active; }

  template <ComponentType T, typename... Args>
  T *addComponent(Args &&...args) {
    const std::size_t typeID = Component::getTypeID<T>();
    const auto it = m_componentMap.find(typeID);
    if (it != m_componentMap.end()) {
      return static_cast<T *>(it->second);
    }

    auto component = std::make_unique<T>(std::forward<Args>(args)...);
    T *ptr = component.get();

    component->setEntity(this);
    if (m_initialized) {
      component->initialize();
    }

    m_componentMap[typeID] = ptr;
    m_components.push_back(std::move(component));
    return ptr;
  }

  template <ComponentType T>
  T *getComponent() const {
    const auto it = m_componentMap.find(Component::getTypeID<T>());
    if (it != m_componentMap.end()) {
      return static_cast<T *>(it->second);
    }
    return nullptr;
  }

  template <ComponentType T>
  bool removeComponent() {
    const std::size_t typeID = Component::getTypeID<T>();
    auto it = m_componentMap.find(typeID);
    if (it == m_componentMap.end()) {
      return false;
    }

    Component *toRemove = it->second;
    m_componentMap.erase(it);

    for (auto componentIt = m_components.begin(); componentIt != m_components.end();
         ++componentIt) {
      if (componentIt->get() == toRemove) {
        m_components.erase(componentIt);
        return true;
      }
    }

    return false;
  }

  template <ComponentType T>
  bool hasComponent() const {
    return m_componentMap.contains(Component::getTypeID<T>());
  }

  const std::vector<std::unique_ptr<Component>> &getComponents() const {
    return m_components;
  }

private:
  std::string m_name;
  bool m_active = true;
  bool m_initialized = false;
  std::vector<std::unique_ptr<Component>> m_components;
  std::unordered_map<std::size_t, Component *> m_componentMap;
};
} // namespace prism
