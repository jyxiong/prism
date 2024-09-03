#include "comet/vulkan/exception.h"

#include <stdexcept>

using namespace comet;

Exception::Exception(const VkResult result, const std::string &msg)
    : m_result{result},
      std::runtime_error{msg}
{
  m_msg = std::string(std::runtime_error::what()) + std::string{" : "} + std::to_string(result);
}

const char *Exception::what() const noexcept
{
  return m_msg.c_str();
}