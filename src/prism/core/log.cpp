using namespace prism::log;

void init() {
  auto logger = spdlog::stdout_color_mt("prism");
  logger->set_pattern("[%^%l%$] %v");
  logger->set_level(spdlog::level::trace);
  spdlog::set_default_logger(logger);
}
