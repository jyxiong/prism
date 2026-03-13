#pragma once

#include <glm/glm.hpp>

#include "prism/component.h"

namespace prism {
class Camera : public Component {

public:
  enum class ProjectionType { Perspective, Orthographic };

public:
  explicit Camera(const std::string &name) : Component(name) {};

  virtual ~Camera() = default;

  void update(std::chrono::microseconds deltaTime) override;

  void setPerspective(float fieldOfView, float aspectRatio, float nearPlane,
                      float farPlane);
  void setOrthographic(float width, float height, float nearPlane,
                       float farPlane);

  void setTarget(const glm::vec3 &target);
  void setUp(const glm::vec3 &up);
  void setAspectRatio(float aspectRatio);

  ProjectionType getProjectionType() const { return m_projectionType; }
  const glm::vec3 &getTarget() const { return m_target; }
  const glm::vec3 &getUp() const { return m_up; }

  const glm::mat4 &getViewMatrix();
  const glm::mat4 &getProjectionMatrix();

private:
  void updateViewMatrix();
  void updateProjectionMatrix();

private:
  ProjectionType m_projectionType = ProjectionType::Perspective;

  glm::vec3 m_target = {0.0f, 0.0f, 0.0f};
  glm::vec3 m_up = {0.0f, 1.0f, 0.0f};

  float m_fieldOfView = 45.0f;
  float m_aspectRatio = 16.0f / 9.0f;

  float m_orthoWidth = 10.0f;
  float m_orthoHeight = 10.0f;

  float m_nearPlane = 0.1f;
  float m_farPlane = 100.0f;

  glm::mat4 m_viewMatrix = glm::mat4(1.0f);
  glm::mat4 m_projectionMatrix = glm::mat4(1.0f);

  bool m_viewMatrixDirty = true;
  bool m_projectionMatrixDirty = true;
};
} // namespace prism
