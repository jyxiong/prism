#pragma once

#include <glm/glm.hpp>

#include "prism/component.h"

namespace prism {
class Transform : public Component {
public:
  explicit Transform(const std::string &name) : Component(name) {};
  virtual ~Transform() = default;

  const glm::vec3 &getPosition() const { return m_position; }

  const glm::vec3 &getRotation() const { return m_rotation; }

  const glm::vec3 &getScale() const { return m_scale; }

  const glm::mat4 &getModelMatrix();

  void setPosition(const glm::vec3 &position) {
    m_position = position;
    m_dirty = true;
  }

  void setRotation(const glm::vec3 &rotation) {
    m_rotation = rotation;
    m_dirty = true;
  }

  void setScale(const glm::vec3 &scale) {
    m_scale = scale;
    m_dirty = true;
  }

  void translate(const glm::vec3 &delta) {
    m_position += delta;
    m_dirty = true;
  }

  void rotate(const glm::vec3 &delta) {
    m_rotation += delta;
    m_dirty = true;
  }

  void scale(const glm::vec3 &delta) {
    m_scale *= delta;
    m_dirty = true;
  }

private:
  void updateModelMatrix();

private:
  glm::vec3 m_position{0.0f, 0.0f, 0.0f};
  glm::vec3 m_rotation{0.0f, 0.0f, 0.0f};
  glm::vec3 m_scale{1.0f, 1.0f, 1.0f};

  glm::mat4 m_modelMatrix{1.0f};
  bool m_dirty = true;
};
} // namespace prism