#pragma once


namespace prism
{
  void check_result(VkResult result, const char *file, int32_t line);
}

#ifndef VK_CHECK
#define VK_CHECK(result) prism::check_result(result, __FILE__, __LINE__)
#endif