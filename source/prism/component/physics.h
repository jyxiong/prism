#pragma once

#include <glm/glm.hpp>

#include "prism/component.h"

namespace prism {

class Physics : public Component {
public:
  explicit Physics(const std::string &name) : Component(name) {};
  virtual ~Physics() = default;

  void update(std::chrono::microseconds deltaTime) override;

  void setVelocity(const glm::vec3 &velocity) { m_velocity = velocity; }
  const glm::vec3 &getVelocity() const { return m_velocity; }

  void setAcceleration(const glm::vec3 &acceleration) {
    m_acceleration = acceleration;
  }
  const glm::vec3 &getAcceleration() const { return m_acceleration; }

  void setUseGravity(bool useGravity) { m_useGravity = useGravity; }
  bool getUseGravity() const { return m_useGravity; }

  void setGravity(const glm::vec3 &gravity) { m_gravity = gravity; }
  const glm::vec3 &getGravity() const { return m_gravity; }

private:
  glm::vec3 m_velocity{0.0f, 0.0f, 0.0f};
  glm::vec3 m_acceleration{0.0f, 0.0f, 0.0f};
  glm::vec3 m_gravity{0.0f, -9.8f, 0.0f};
  bool m_useGravity = false;
};

} // namespace prism