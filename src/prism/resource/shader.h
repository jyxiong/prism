#pragma once

#include <string>
#include <vector>

#include <volk.h>

#include "prism/resource/resource.h"

namespace prism {

class ShaderResource : public Resource {
public:
  ShaderResource(const std::string& id,
         VkShaderStageFlagBits stage = VK_SHADER_STAGE_VERTEX_BIT);
  ~ShaderResource() override;

  VkShaderModule getShaderModule() const;
  VkShaderStageFlagBits getStage() const;

protected:
  bool doLoad() override;
  bool doUnload() override;

private:
  std::string getShaderExtension() const;
  bool readFile(const std::string& filePath, std::vector<char>& buffer);
  bool createShaderModule(const std::vector<char>& code);
  void reset();

  VkShaderModule m_shaderModule{VK_NULL_HANDLE};
  VkShaderStageFlagBits m_stage;
};

} // namespace prism