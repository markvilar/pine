#pragma once
#include "Sennet/pch.hpp"

#include "Sennet/Core/Base.hpp"
#include "Sennet/Core/Timestep.hpp"

#include "Sennet/Events/Event.hpp"

#include "Sennet/Messages/Message.hpp"

namespace Sennet
{

class Layer
{
public:
	Layer(const std::string& name = "layer");
	virtual ~Layer() = default;

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate(Timestep ts) {}
	virtual void OnImGuiRender() {}
	virtual void OnEvent(Event& e) {}
	virtual void OnMessage(Ref<Message> msg) {}

	inline const std::string& GetName() const { return m_DebugName; }

protected:
	std::string m_DebugName;
};

}
