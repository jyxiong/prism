#pragma once

namespace prism
{

    class Application
    {
    public:
        Application(const std::string& name);

        virtual ~Application() = default;

        const std::string& get_name() const;

    private:
        std::string m_name;
    };

} // namespace prism