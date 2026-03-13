#pragma once

#include <chrono>
#include <concepts>

namespace prism {
class World;

class System {
public:
  virtual ~System() = default;

  virtual void initialize(World &world) {}
  virtual void update(World &world, std::chrono::microseconds deltaTime) {}
  virtual void render(World &world) {}

  void setActive(bool active) { m_active = active; }
  bool getActive() const { return m_active; }

private:
  bool m_active = true;
};

template <typename T>
concept SystemType = std::derived_from<T, System>;

} // namespace prism
