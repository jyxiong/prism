#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <volk.h>

#include "prism/resource/resource.h"

namespace prism {

class MeshResource : public Resource {
public:
  struct Vertex {
    float position[3]{};
    float normal[3]{};
    float uv[2]{};
  };

  explicit MeshResource(const std::string& id);
  ~MeshResource() override;

  VkBuffer getVertexBuffer() const;
  VkDeviceMemory getVertexBufferMemory() const;
  VkDeviceSize getVertexBufferOffset() const;
  std::uint32_t getVertexCount() const;

  VkBuffer getIndexBuffer() const;
  VkDeviceMemory getIndexBufferMemory() const;
  VkDeviceSize getIndexBufferOffset() const;
  std::uint32_t getIndexCount() const;

protected:
  bool doLoad() override;
  bool doUnload() override;

private:
  bool loadMeshData(const std::string& filePath, std::vector<Vertex>& vertices,
                    std::vector<std::uint32_t>& indices);
  bool createVertexBuffer(const std::vector<Vertex>& vertices);
  bool createIndexBuffer(const std::vector<std::uint32_t>& indices);
  void reset();

  VkBuffer m_vertexBuffer{VK_NULL_HANDLE};
  VkDeviceMemory m_vertexBufferMemory{VK_NULL_HANDLE};
  VkDeviceSize m_vertexBufferOffset{0};
  std::uint32_t m_vertexCount{0};

  VkBuffer m_indexBuffer{VK_NULL_HANDLE};
  VkDeviceMemory m_indexBufferMemory{VK_NULL_HANDLE};
  VkDeviceSize m_indexBufferOffset{0};
  std::uint32_t m_indexCount{0};
};

} // namespace prism