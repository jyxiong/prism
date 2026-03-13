#include "prism/world.h"

using namespace prism;

Entity *World::createEntity(const std::string &name) {
  auto entity = std::make_unique<Entity>(name);
  Entity *ptr = entity.get();

  if (m_initialized) {
    ptr->initialize();
  }

  m_entities.push_back(std::move(entity));
  return ptr;
}

bool World::removeEntity(const std::string &name) {
  for (auto it = m_entities.begin(); it != m_entities.end(); ++it) {
    if ((*it)->getName() == name) {
      m_entities.erase(it);
      return true;
    }
  }
  return false;
}

Entity *World::findEntity(const std::string &name) const {
  for (const auto &entity : m_entities) {
    if (entity->getName() == name) {
      return entity.get();
    }
  }
  return nullptr;
}

void World::initialize() {
  if (m_initialized) {
    return;
  }

  for (const auto &entity : m_entities) {
    entity->initialize();
  }

  for (const auto &system : m_systems) {
    system->initialize(*this);
  }

  m_initialized = true;
}

void World::update(std::chrono::microseconds deltaTime) {
  if (!m_initialized) {
    initialize();
  }

  for (const auto &system : m_systems) {
    if (system->getActive()) {
      system->update(*this, deltaTime);
    }
  }

  for (const auto &entity : m_entities) {
    entity->update(deltaTime);
  }
}

void World::render() {
  if (!m_initialized) {
    initialize();
  }

  for (const auto &system : m_systems) {
    if (system->getActive()) {
      system->render(*this);
    }
  }

  for (const auto &entity : m_entities) {
    entity->render();
  }
}
