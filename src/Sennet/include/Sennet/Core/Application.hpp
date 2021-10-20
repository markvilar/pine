#pragma once

#include "Sennet/Core/Base.hpp"

#include "Sennet/Core/LayerStack.hpp"
#include "Sennet/Core/Timestep.hpp"
#include "Sennet/Core/Window.hpp"

#include "Sennet/Events/ApplicationEvent.hpp"
#include "Sennet/Events/Event.hpp"

#include "Sennet/ImGui/ImGuiLayer.hpp"

#include "Sennet/Renderer/Renderer.hpp"

int main(int argc, char** argv);

namespace Sennet
{

class Application
{
public:
    struct Specification
    {
        std::string WorkingDirectory;
        std::string Name = "Sennet App";
        uint32_t WindowWidth = 1600;
        uint32_t WindowHeight = 800;
        bool StartMaximized = true;
        bool VSync = true;
        bool Resizable = true;
        bool EnableImGui = true;
        bool Fullscreen = false;
    };

public:
    Application(const Specification& specs);
    virtual ~Application();

    void Run();
    void Close();

    virtual void OnInit() {}
    virtual void OnShutdown() {}
    virtual void OnUpdate() {}

    void OnEvent(Event& e);

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* overlay);
    void PopLayer(Layer* layer);
    void PopOverlay(Layer* overlay);
    void RenderImGui();

    inline Window& GetWindow() { return *m_Window; }

    ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

    static inline Application& Get() { return *s_Instance; }

    const Specification& GetSpecification() const { return m_Specification; }

private:
    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);
    bool OnWindowIconify(WindowIconifyEvent& e);

private:
    std::unique_ptr<Window> m_Window;
    Specification m_Specification;

    ImGuiLayer* m_ImGuiLayer;
    LayerStack m_LayerStack;

    bool m_Running = true;
    bool m_Minimized = false;
    float m_LastFrameTime = 0.0f;
    Timestep m_Timestep;

    static Application* s_Instance;
};

// To be defined in client.
Application* CreateApplication(int argc, char** argv);

} // namespace Sennet
