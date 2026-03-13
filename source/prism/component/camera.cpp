#include "prism/component/camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "prism/component/transform.h"
#include "prism/entity.h"

using namespace prism;

void Camera::update(std::chrono::microseconds /*deltaTime*/) {
  // Transform may change every frame, so mark cached view matrix dirty.
  m_viewMatrixDirty = true;
}

void Camera::setPerspective(float fieldOfView, float aspectRatio, float nearPlane,
                            float farPlane) {
  m_projectionType = ProjectionType::Perspective;
  m_fieldOfView = fieldOfView;
  m_aspectRatio = aspectRatio;
  m_nearPlane = nearPlane;
  m_farPlane = farPlane;
  m_projectionMatrixDirty = true;
}

void Camera::setOrthographic(float width, float height, float nearPlane,
                             float farPlane) {
  m_projectionType = ProjectionType::Orthographic;
  m_orthoWidth = width;
  m_orthoHeight = height;
  m_nearPlane = nearPlane;
  m_farPlane = farPlane;
  m_projectionMatrixDirty = true;
}

void Camera::setTarget(const glm::vec3 &target) {
  m_target = target;
  m_viewMatrixDirty = true;
}

void Camera::setUp(const glm::vec3 &up) {
  m_up = up;
  m_viewMatrixDirty = true;
}

void Camera::setAspectRatio(float aspectRatio) {
  m_aspectRatio = aspectRatio;
  m_projectionMatrixDirty = true;
}

const glm::mat4 &Camera::getViewMatrix() {
  if (m_viewMatrixDirty) {
    updateViewMatrix();
  }
  return m_viewMatrix;
}

const glm::mat4 &Camera::getProjectionMatrix() {
  if (m_projectionMatrixDirty) {
    updateProjectionMatrix();
  }
  return m_projectionMatrix;
}

void Camera::updateViewMatrix() {
  glm::vec3 cameraPosition{0.0f, 0.0f, 0.0f};

  if (const auto *entity = getEntity()) {
    if (const auto *transform = entity->getComponent<Transform>()) {
      cameraPosition = transform->getPosition();
    }
  }

  m_viewMatrix = glm::lookAt(cameraPosition, m_target, m_up);
  m_viewMatrixDirty = false;
}

void Camera::updateProjectionMatrix() {
  if (m_projectionType == ProjectionType::Perspective) {
    m_projectionMatrix =
        glm::perspective(glm::radians(m_fieldOfView), m_aspectRatio, m_nearPlane,
                         m_farPlane);
  } else {
    const float halfWidth = m_orthoWidth * 0.5f;
    const float halfHeight = m_orthoHeight * 0.5f;
    m_projectionMatrix = glm::ortho(-halfWidth, halfWidth, -halfHeight,
                                    halfHeight, m_nearPlane, m_farPlane);
  }

  // Vulkan clip space has inverted Y compared to OpenGL conventions.
  m_projectionMatrix[1][1] *= -1.0f;
  m_projectionMatrixDirty = false;
}
