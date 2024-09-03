#pragma once

#include "spdlog/spdlog.h"

namespace comet
{
    class Log
    {
    public:
        static void init();

        static std::shared_ptr<spdlog::logger> &get_logger();

    private:
        static std::shared_ptr<spdlog::logger> s_logger;
    };
}

#define LOG_TRACE(...) ::comet::Log::get_logger()->trace(__VA_ARGS__)
#define LOG_INFO(...) ::comet::Log::get_logger()->info(__VA_ARGS__)
#define LOG_WARN(...) ::comet::Log::get_logger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::comet::Log::get_logger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::comet::Log::get_logger()->critical(__VA_ARGS)