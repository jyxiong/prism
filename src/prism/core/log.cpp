#include "prism/core/log.h"

using namespace prism;

std::shared_ptr<spdlog::logger> Log::s_logger;

std::shared_ptr<spdlog::logger> &Log::get_logger()
{
    return s_logger;
}

void Log::init()
{
    spdlog::set_pattern("%^[%T] %n: %v%$");
    s_logger = spdlog::stdout_color_mt("prism");
    s_logger->set_level(spdlog::level::trace);
}
