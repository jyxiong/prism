#pragma once

#include <concepts>
#include <string>

namespace prism {

class Resource {
public:
  Resource(const std::string& id);
  virtual ~Resource() = default;

  const std::string& getId() const;
  bool isLoaded() const;

  bool load();
  bool unload();

protected:
  virtual bool doLoad() = 0;
  virtual bool doUnload() = 0;

private:
  std::string m_id;
  bool m_loaded;
};

template<typename T>
concept ResourceType = std::derived_from<T, Resource>;
}