#pragma once

#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

#include "prism/component.h"

namespace prism {

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoord;
  glm::vec4 tangent;
};

class Mesh : public Component {
public:
  explicit Mesh(const std::string &name) : Component(name) {};
  virtual ~Mesh() = default;

  void setVertices(const std::vector<Vertex> &vertices) {
    m_vertices = vertices;
  }

  void setIndices(const std::vector<uint32_t> &indices) {
    m_indices = indices;
  }

  const std::vector<Vertex> &getVertices() const { return m_vertices; }
  const std::vector<uint32_t> &getIndices() const { return m_indices; }

  std::size_t getVertexCount() const { return m_vertices.size(); }
  std::size_t getIndexCount() const { return m_indices.size(); }

private:
  std::vector<Vertex> m_vertices;
  std::vector<uint32_t> m_indices;
};

} // namespace prism