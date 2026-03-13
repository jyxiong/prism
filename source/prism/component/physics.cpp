#include "prism/component/physics.h"

#include "prism/component/transform.h"
#include "prism/entity.h"

using namespace prism;

void Physics::update(std::chrono::microseconds deltaTime) {
  Entity *entity = getEntity();
  if (!entity) {
    return;
  }

  Transform *transform = entity->getComponent<Transform>();
  if (!transform) {
    return;
  }

  const float deltaSeconds = std::chrono::duration<float>(deltaTime).count();

  glm::vec3 frameAcceleration = m_acceleration;
  if (m_useGravity) {
    frameAcceleration += m_gravity;
  }

  m_velocity += frameAcceleration * deltaSeconds;
  transform->translate(m_velocity * deltaSeconds);
}
