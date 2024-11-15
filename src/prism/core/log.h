#pragma once

namespace prism::log {

void init();

} // namespace prism::log

#define LOG_TRACE(...) spdlog::trace(__VA_ARGS__);
#define LOG_INFO(...) spdlog::info(__VA_ARGS__);
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__);
#define LOG_ERROR(...) spdlog::error("{}", fmt::format(__VA_ARGS__));
#define LOG_CRITICAL(...) spdlog::debug(__VA_ARGS__);
