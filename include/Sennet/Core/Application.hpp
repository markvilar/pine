#pragma once

#include <Sennet/Core/Base.hpp>

#include <Sennet/Core/Window.hpp>
#include <Sennet/Core/LayerStack.hpp>

#include <Sennet/Events/Event.hpp>
#include <Sennet/Events/ApplicationEvent.hpp>

#include <Sennet/ImGui/ImGuiLayer.hpp>

// Temporary.
#include <Sennet/Renderer/Buffer.hpp>
#include <Sennet/Renderer/Shader.hpp>
#include <Sennet/Renderer/VertexArray.hpp>

namespace Sennet
{

// TODO: Look into an alternative to Sennet::main function!
int main(int argc, char** argv);

class Application
{
public:
	Application();
	virtual ~Application();

	void OnEvent(Event& e);

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);

	Window& GetWindow() { return *m_Window; }

	void Close();

	static Application& Get() { return *s_Instance; }

private:
	void Run();
	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);

private:
	Scope<Window> m_Window;
	ImGuiLayer* m_ImGuiLayer;
	bool m_Running = true;
	LayerStack m_LayerStack;

	// Temporary.
	Ref<Shader> m_Shader;
	Ref<VertexArray> m_TriangleVa;

	Ref<Shader> m_BlueShader;
	Ref<VertexArray> m_SquareVa;
private:
	static Application* s_Instance;
	friend int main(int argc, char** argv);
};

// To be defined in client.
Application* CreateApplication();

}