#pragma once

namespace prism
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

#define LOG_TRACE(...) ::prism::Log::get_logger()->trace(__VA_ARGS__)
#define LOG_INFO(...) ::prism::Log::get_logger()->info(__VA_ARGS__)
#define LOG_WARN(...) ::prism::Log::get_logger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::prism::Log::get_logger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::prism::Log::get_logger()->critical(__VA_ARGS)