#include "renderer.h"

using namespace prism;

int main() {
  if (volkInitialize()) {
    throw std::runtime_error("Failed to initialize volk.");
  }
  
    Renderer render;
    render.render_loop();
    // render.save("output.hdr");

    return 0;
}