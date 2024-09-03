#pragma once

#include "volk.h"

namespace prism
{
  class Exception : public std::runtime_error
  {
  public:
    Exception(VkResult result, const std::string &msg = "Vulkan error");

    const char *what() const noexcept override;

  private:
    VkResult m_result;

    std::string m_msg;
  };
}