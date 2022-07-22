#pragma once

#include <memory>

#include "pine/pine.hpp"

namespace pine
{

struct InterfaceLayout
{
    Vec2 Position = {0.0f, 0.0f};
    Vec2 Size = {0.0f, 0.0f};

    InterfaceLayout() = default;
    InterfaceLayout(const InterfaceLayout&) = default;
    InterfaceLayout(InterfaceLayout&&) = default;

    InterfaceLayout(const Vec2& position, const Vec2& size)
        : Position(position), Size(size)
    {
    }

    ~InterfaceLayout() = default;

    InterfaceLayout& operator=(const InterfaceLayout&) = default;
    InterfaceLayout& operator=(InterfaceLayout&&) = default;
};

class EditorLayer : public Layer
{
public:
    EditorLayer();
    ~EditorLayer() = default;

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_update(Timestep ts) override;

    virtual void on_imgui_render() override;

    virtual void on_event(Event& event) override;

private:
    void UpdateInterfaceLayout();

private:
    OrthographicCameraController m_CameraController{1.0f};
    ShaderLibrary m_ShaderLibrary{};

    Image m_Image;

    ClientState m_client{};
    ServerState m_server{5000};

    std::vector<std::vector<uint8_t>> m_server_history{};

    std::shared_ptr<Framebuffer> m_viewport_framebuffer;
    std::shared_ptr<Texture2D> m_Texture;

    bool m_ViewportFocused{false};
    bool m_ViewportHovered{false};

    Renderer2D::RendererData m_RendererData2D{};

    Vec4 m_QuadColor{0.2f, 0.3f, 0.8f, 1.0f};
    float m_QuadRotation{0.0f};

    std::unordered_map<std::string, InterfaceLayout> m_InterfaceLayouts{};
};

} // namespace pine
