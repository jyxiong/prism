#pragma once

#include <cstddef>
#include <chrono>
#include <concepts>
#include <string>

namespace prism {

class Entity;

class ComponentTypeIDSystem {
public:
  template <typename T>
  static std::size_t getTypeID() {
    static const std::size_t typeID = s_nextTypeID++;
    return typeID;
  }

private:
  static std::size_t s_nextTypeID;
};

class Component {
public:
  explicit Component(const std::string &name) : m_name(name) {}

  virtual ~Component() = default;

  virtual void initialize() {}

  virtual void update(std::chrono::microseconds deltaTime) {}

  virtual void render() {}

  void setName(const std::string &name) { m_name = name; }
  const std::string &getName() const { return m_name; }

  void setActive(bool active) { m_active = active; }
  bool getActive() const { return m_active; }

  void setEntity(Entity *entity) { m_entity = entity; }
  Entity *getEntity() const { return m_entity; }

  template <typename T>
  static std::size_t getTypeID() {
    return ComponentTypeIDSystem::getTypeID<T>();
  }

protected:
  std::string m_name;
  bool m_active = true;
  Entity *m_entity = nullptr;
};

template <typename T>
concept ComponentType = std::derived_from<T, Component>;

} // namespace prism