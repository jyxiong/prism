#include "prism/component/transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace prism;

const glm::mat4 &Transform::getModelMatrix() {
  if (m_dirty) {
    updateModelMatrix();
  }
  return m_modelMatrix;
}

void Transform::updateModelMatrix() {
  auto translation = glm::translate(glm::mat4(1.0f), m_position);

  auto rotationX =
      glm::angleAxis(glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
  auto rotationY =
      glm::angleAxis(glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
  auto rotationZ =
      glm::angleAxis(glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
  auto rotation = glm::mat4_cast(rotationY * rotationX * rotationZ);

  auto scale = glm::scale(glm::mat4(1.0f), m_scale);

  m_modelMatrix = translation * rotation * scale;
  m_dirty = false;
}