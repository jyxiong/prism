#include "prism/resource/mesh.h"

using namespace prism;

MeshResource::MeshResource(const std::string& id) : Resource(id) {}

MeshResource::~MeshResource() {
  unload();
}

VkBuffer MeshResource::getVertexBuffer() const {
  return m_vertexBuffer;
}

VkDeviceMemory MeshResource::getVertexBufferMemory() const {
  return m_vertexBufferMemory;
}

VkDeviceSize MeshResource::getVertexBufferOffset() const {
  return m_vertexBufferOffset;
}

std::uint32_t MeshResource::getVertexCount() const {
  return m_vertexCount;
}

VkBuffer MeshResource::getIndexBuffer() const {
  return m_indexBuffer;
}

VkDeviceMemory MeshResource::getIndexBufferMemory() const {
  return m_indexBufferMemory;
}

VkDeviceSize MeshResource::getIndexBufferOffset() const {
  return m_indexBufferOffset;
}

std::uint32_t MeshResource::getIndexCount() const {
  return m_indexCount;
}

bool MeshResource::doLoad() {
  std::vector<Vertex> vertices;
  std::vector<std::uint32_t> indices;
  std::string filePath = "models/" + getId() + ".gltf";

  if (!loadMeshData(filePath, vertices, indices)) {
    return false;
  }

  if (!createVertexBuffer(vertices) || !createIndexBuffer(indices)) {
    reset();
    return false;
  }

  m_vertexCount = static_cast<std::uint32_t>(vertices.size());
  m_indexCount = static_cast<std::uint32_t>(indices.size());
  return true;
}

bool MeshResource::doUnload() {
  reset();
  return true;
}

bool MeshResource::loadMeshData(const std::string& filePath,
                        std::vector<Vertex>& vertices,
                        std::vector<std::uint32_t>& indices) {
  (void)filePath;
  (void)vertices;
  (void)indices;
  return false;
}

bool MeshResource::createVertexBuffer(const std::vector<Vertex>& vertices) {
  (void)vertices;
  return false;
}

bool MeshResource::createIndexBuffer(const std::vector<std::uint32_t>& indices) {
  (void)indices;
  return false;
}

void MeshResource::reset() {
  m_vertexBuffer = VK_NULL_HANDLE;
  m_vertexBufferMemory = VK_NULL_HANDLE;
  m_vertexBufferOffset = 0;
  m_vertexCount = 0;
  m_indexBuffer = VK_NULL_HANDLE;
  m_indexBufferMemory = VK_NULL_HANDLE;
  m_indexBufferOffset = 0;
  m_indexCount = 0;
}