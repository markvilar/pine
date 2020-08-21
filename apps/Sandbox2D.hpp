#pragma once
#include <Sennet/Sennet.hpp>

class Sandbox2D : public Sennet::Layer
{
public:
	Sandbox2D();
	~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(Sennet::Timestep ts) override;

	virtual void OnImGuiRender() override;

	virtual void OnEvent(Sennet::Event& e) override;

private:
	Sennet::OrthographicCameraController m_CameraController;

	Sennet::Ref<Sennet::Shader> m_FlatColorShader;
	Sennet::Ref<Sennet::VertexArray> m_SquareVA;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};
