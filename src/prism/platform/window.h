#pragma once

namespace prism
{
    class Instance;

    class Window
    {
    public:
        enum class Mode
        {
            Headless,
            Fullscreen,
            FullscreenBorderless,
            FullscreenStretch,
            Default
        };

        enum class Vsync
        {
            OFF,
            ON,
            Default
        };

        struct Properties
        {
            std::string title = "";
            Mode mode = Mode::Default;
            bool resizable = true;
            Vsync vsync = Vsync::Default;
            glm::ivec2 extent = {1280, 720};
        };

    public:
        Window(const Properties &properties);

        virtual ~Window() = default;

        virtual void* get_handle() const = 0;

        virtual bool should_close() = 0;

        virtual void process_events() = 0;

        virtual void close() = 0;

        virtual std::vector<const char *> get_required_extensions() const = 0;

        virtual VkSurfaceKHR create_surface(const Instance& instance) const = 0;

        glm::ivec2 resize(const glm::ivec2 &extent);

    private:
        Properties m_properties;

    }; // class Window
} // namespace prism