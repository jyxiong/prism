#include "prism/resource/resource.h"

using namespace prism;

Resource::Resource(const std::string& id) : m_id(id), m_loaded(false) {}

const std::string& Resource::getId() const {
  return m_id;
}

bool Resource::isLoaded() const {
  return m_loaded;
}

bool Resource::load() {
  if (m_loaded) {
    return true;
  }

  m_loaded = doLoad();
  return m_loaded;
}

bool Resource::unload() {
  if (!m_loaded) {
    return true;
  }

  if (!doUnload()) {
    return false;
  }

  m_loaded = false;
  return true;
}