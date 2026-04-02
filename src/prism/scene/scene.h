#pragma once

#include <entt/entt.hpp>

namespace prism {
class Scene {
public:
  Scene() = default;

  ~Scene() = default;

  entt::registry &getRegistry();
  const entt::registry &getRegistry() const;

private:
  entt::registry m_registry;
};
} // namespace prism