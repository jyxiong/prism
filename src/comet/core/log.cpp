#include "comet/core/log.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

using namespace comet;

std::shared_ptr<spdlog::logger> Log::s_logger;

std::shared_ptr<spdlog::logger> &Log::get_logger()
{
    return s_logger;
}

void Log::init()
{
    spdlog::set_pattern("%^[%T] %n: %v%$");
    s_logger = spdlog::stdout_color_mt("COMET");
    s_logger->set_level(spdlog::level::trace);
}
