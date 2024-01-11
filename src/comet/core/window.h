#pragma once

#include <string>
#include <vector>

namespace comet
{
    class Window
    {
    public:
        struct Extent
        {
            uint32_t width;
            uint32_t height;
        };

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
            Extent extent = {1280, 720};
        };

    public:
        Window(const Properties &properties);

        virtual ~Window() = default;

        virtual bool should_close() = 0;

        virtual void process_events() = 0;

        virtual void close() = 0;

        virtual std::vector<const char *> get_required_extensions() const = 0;

        Extent resize(const Extent &extent);

    private:
        Properties m_properties;

    }; // class Window
} // namespace comet