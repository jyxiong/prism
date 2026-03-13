#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "prism/component.h"
#include "prism/entity.h"
#include "prism/system.h"

namespace prism {
class World {
public:
  World() = default;
  ~World() = default;

  Entity *createEntity(const std::string &name);
  bool removeEntity(const std::string &name);
  Entity *findEntity(const std::string &name) const;

  template <SystemType T, typename... Args>
  T *addSystem(Args &&...args) {
    const std::type_index key = std::type_index(typeid(T));
    const auto it = m_systemMap.find(key);
    if (it != m_systemMap.end()) {
      return static_cast<T *>(it->second);
    }

    auto system = std::make_unique<T>(std::forward<Args>(args)...);
    T *ptr = system.get();

    if (m_initialized) {
      ptr->initialize(*this);
    }

    m_systemMap[key] = ptr;
    m_systems.push_back(std::move(system));
    return ptr;
  }

  template <SystemType T>
  T *getSystem() const {
    const auto it = m_systemMap.find(std::type_index(typeid(T)));
    if (it != m_systemMap.end()) {
      return static_cast<T *>(it->second);
    }
    return nullptr;
  }

  template <ComponentType... Ts>
  std::vector<Entity *> queryEntities() const {
    std::vector<Entity *> result;
    for (const auto &entity : m_entities) {
      if (((entity->hasComponent<Ts>()) && ...)) {
        result.push_back(entity.get());
      }
    }
    return result;
  }

  const std::vector<std::unique_ptr<Entity>> &getEntities() const {
    return m_entities;
  }

  void initialize();
  void update(std::chrono::microseconds deltaTime);
  void render();

private:
  bool m_initialized = false;
  std::vector<std::unique_ptr<Entity>> m_entities;
  std::vector<std::unique_ptr<System>> m_systems;
  std::unordered_map<std::type_index, System *> m_systemMap;
};

} // namespace prism
