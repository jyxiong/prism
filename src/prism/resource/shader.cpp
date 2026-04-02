#include "prism/resource/shader.h"

#include <string>

using namespace prism;

ShaderResource::ShaderResource(const std::string& id, VkShaderStageFlagBits stage)
    : Resource(id), m_stage(stage) {}

ShaderResource::~ShaderResource() {
  unload();
}

VkShaderModule ShaderResource::getShaderModule() const {
  return m_shaderModule;
}

VkShaderStageFlagBits ShaderResource::getStage() const {
  return m_stage;
}

bool ShaderResource::doLoad() {
  std::vector<char> shaderCode;
  std::string filePath = "shaders/" + getId() + getShaderExtension() + ".spv";
  if (!readFile(filePath, shaderCode)) {
    return false;
  }

  if (!createShaderModule(shaderCode)) {
    reset();
    return false;
  }

  return true;
}

bool ShaderResource::doUnload() {
  reset();
  return true;
}

std::string ShaderResource::getShaderExtension() const {
  switch (m_stage) {
    case VK_SHADER_STAGE_VERTEX_BIT:
      return ".vert";
    case VK_SHADER_STAGE_FRAGMENT_BIT:
      return ".frag";
    case VK_SHADER_STAGE_COMPUTE_BIT:
      return ".comp";
    default:
      return "";
  }
}

bool ShaderResource::readFile(const std::string& filePath, std::vector<char>& buffer) {
  (void)filePath;
  (void)buffer;
  return false;
}

bool ShaderResource::createShaderModule(const std::vector<char>& code) {
  (void)code;
  return false;
}

void ShaderResource::reset() {
  m_shaderModule = VK_NULL_HANDLE;
}