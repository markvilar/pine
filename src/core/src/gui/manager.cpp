#include "pine/gui/manager.hpp"

// FIXME: GUI implementation abstraction
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "pine/pch.hpp"
#include "pine/gui/common.hpp"

namespace pine::gui
{

std::unique_ptr<Context> create_context(Window* window)
{
    return std::make_unique<Context>(window);
}

std::unique_ptr<IO> create_io(Context* context)
{
    return std::make_unique<IO>(context);
}

std::unique_ptr<Manager> create_manager(Window* window)
{
    auto context = create_context(window);
    auto io = create_io(context.get());
    return std::make_unique<Manager>(context, io);
}

// ----------------------------------------------------------------------------
// Context
// ----------------------------------------------------------------------------

Context::Context(Window* window_)
    : window(window_)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
}

Context::~Context()
{
    ImGui::DestroyContext();
}

void Context::init() const
{
    auto native_window = static_cast<GLFWwindow*>(window->get_native_window());
    ImGui_ImplGlfw_InitForOpenGL(native_window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
}

void Context::shutdown() const
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
}

void Context::begin_frame() const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Context::end_frame() const
{
    auto& io = ImGui::GetIO();

    if (window)
    {
        io.DisplaySize = ImVec2(static_cast<float>(window->get_width()),
            static_cast<float>(window->get_height()));
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ConfigFlagOptions::VIEWPORTS_ENABLE)
    {
        auto backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

// ----------------------------------------------------------------------------
// IO
// ----------------------------------------------------------------------------

IO::IO(Context* context_)
    : context(context_)
{
    auto& io = ImGui::GetIO();
    io.WantSaveIniSettings = false;
}

IO::~IO()
{
}

ConfigFlags IO::get_config_flags() const
{
    return ImGui::GetIO().ConfigFlags;
}

void IO::set_config_flags(const ConfigFlags& config) const
{
    ImGui::GetIO().ConfigFlags = config;
}

bool IO::load_settings(const std::filesystem::path& filepath) const
{
    ImGui::LoadIniSettingsFromDisk(filepath.c_str());
    return false;
}

bool IO::save_settings(const std::filesystem::path& filepath) const
{
    ImGui::SaveIniSettingsToDisk(filepath.c_str());
    return false;
}

bool IO::want_capture_mouse() const
{
    return ImGui::GetIO().WantCaptureMouse;
}

bool IO::want_capture_keyboard() const
{
    return ImGui::GetIO().WantCaptureKeyboard;
}

// ----------------------------------------------------------------------------
// Manager
// ----------------------------------------------------------------------------

Manager::Manager(std::unique_ptr<Context>& context_, std::unique_ptr<IO>& io_)
    : context(std::move(context_)), io(std::move(io_))
{
    auto config_flags = io->get_config_flags();
    config_flags |= ConfigFlagOptions::ENABLE_KEYBOARD;
    config_flags |= ConfigFlagOptions::DOCKING_ENABLE;
    config_flags |= ConfigFlagOptions::VIEWPORTS_ENABLE;
    io->set_config_flags(config_flags);
    
    // Set dark theme by default
    auto& style = ImGui::GetStyle();
    set_dark_theme(style);

    if (context)
    {
        context->init();
    }
}

Manager::~Manager()
{
    if (context)
    {
        context->shutdown();
    }
}

void Manager::begin_frame() const
{
    if (context)
    {
        context->begin_frame();
    }
}

void Manager::end_frame() const
{
    if (context)
    {
        context->end_frame();
    }
}

bool Manager::load_settings(const std::filesystem::path& filepath) const
{
    if (io)
    {
        return io->load_settings(filepath);
    }
    return false;
}

bool Manager::save_settings(const std::filesystem::path& filepath) const
{
    if (io)
    {
        return io->save_settings(filepath);
    }
    return false;
}

void Manager::on_event(Event& event) const
{
    if (handle_event)
    {
        event.handled |= event.is_in_category(EventCategoryMouse) 
            & io->want_capture_mouse();
        event.handled |= event.is_in_category(EventCategoryKeyboard)
            & io->want_capture_keyboard();
    }
}

} // namespace pine::gui
