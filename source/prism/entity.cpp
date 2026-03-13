#include "prism/entity.h"

using namespace prism;

void Entity::initialize() {
  if (m_initialized) {
    return;
  }

  for (const auto &component : m_components) {
    component->initialize();
  }

  m_initialized = true;
}

void Entity::update(std::chrono::microseconds deltaTime) {
  if (!m_active) {
    return;
  }

  for (const auto &component : m_components) {
    if (component->getActive()) {
      component->update(deltaTime);
    }
  }
}

void Entity::render() {
  if (!m_active) {
    return;
  }

  for (const auto &component : m_components) {
    if (component->getActive()) {
      component->render();
    }
  }
}