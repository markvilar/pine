#pragma once

#include <cstdint>
#include <string>

#include "pine/core/event.hpp"
#include "pine/core/layer.hpp"
#include "pine/core/log.hpp"
#include "pine/core/timestep.hpp"
#include "pine/core/window.hpp"
#include "pine/gui/manager.hpp"
#include "pine/renderer/renderer.hpp"

int main(int argc, char** argv);

namespace pine
{

struct ApplicationSpecs
{
    std::string name = "Default App";
    uint32_t window_width = 1600;
    uint32_t window_height = 800;
    bool fullscreen = false;
    bool vsync = true;

    std::string working_directory;
    bool start_maximized = true;
    bool resizable = true;
    bool enable_gui = true;
};

class Application
{
public:
    Application(const ApplicationSpecs& specs);
    virtual ~Application();

    void run();
    void close();

    virtual void on_init() {}
    virtual void on_shutdown() {}
    virtual void on_update() {}

    void on_event(const Event& event);

    void push_layer(Layer* layer);
    void push_overlay(Layer* overlay);
    void pop_layer(Layer* layer);
    void pop_overlay(Layer* overlay);

    void render_gui();

    inline Window& get_window() const { return *window; }
    inline gui::Manager& get_gui() const { return *gui; }

    static inline Application& get() { return *instance; }

    inline const ApplicationSpecs& get_specification() const
    {
        return specification;
    }

private:
    void on_window_close(const WindowClosed& event);
    void on_window_resize(const WindowResized& event);
    void on_window_iconify(const WindowIconified& event);

private:
    ApplicationSpecs specification;

    std::unique_ptr<Window> window;
    std::unique_ptr<gui::Manager> gui;

    LayerStack layer_stack;

    bool running = true;
    bool minimized = false;
    float last_frame_time = 0.0f;

    static Application* instance;
};

class ApplicationFactory
{
    /*
    Factory for the application class. To be implemented by the client.
    */

public:
    virtual ~ApplicationFactory() = default;

    virtual std::unique_ptr<Application> create_application() = 0;
};

} // namespace pine
