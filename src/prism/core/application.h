#pragma once

#include "prism/vulkan/device.h"
#include "prism/vulkan/instance.h"
#include "prism/vulkan/surface.h"
#include "prism/vulkan/swapchain.h"

namespace prism {

class Application {
public:
  struct Config {
    Window::Properties windowProps;
  };

public:
  static Application &getInstance() { return *s_instance; }

  Application(const Config& config);

  virtual ~Application() = default;

  const Window& getWindow() const { return *m_window; }
  const Device& getDevice() const { return *m_device; }
  const CommandPool& getCommandPool() const { return *m_cmdPool; }

private:
  static Application *s_instance;

  std::unique_ptr<Window> m_window;

  std::unique_ptr<Instance> m_instance;

  std::unique_ptr<Device> m_device;

  std::unique_ptr<Surface> m_surface;

  int m_queueFamilyIndex;

  std::unique_ptr<CommandPool> m_cmdPool;
};

} // namespace prism