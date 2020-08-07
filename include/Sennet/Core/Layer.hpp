#pragma once

#include <Sennet/pch.hpp>

#include <Sennet/Core/Base.hpp>
#include <Sennet/Events/Event.hpp>

namespace Sennet
{

class Layer
{
public:
	Layer(const std::string& name = "layer");
	virtual ~Layer() = default;

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate() {}
	virtual void OnImGuiRender() {}
	virtual void OnEvent(Event& e) {}

	inline const std::string& GetName() const { return m_DebugName; }

protected:
	std::string m_DebugName;
};

}
