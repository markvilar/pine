#pragma once

#include <string>
#include <tuple>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>

#include <sennet/am/core.hpp>

namespace am {
namespace action {

class response : public base_action
{
	typedef boost::asio::ip::tcp boost_tcp;
public:
	// Default constructor.
	response() 
		: m_sender_addr(""),
		m_sender_port(0)
	{}

	// Copy constructor.
	response(const response& other)
	{
		m_sender_addr = other.m_sender_addr;
		m_sender_port = other.m_sender_port;
	}

	// Constructor.
	response(
		const std::string sender_addr, 
		const unsigned short sender_port
		)
	{
		m_sender_addr = sender_addr;
		m_sender_port = sender_port;
	}

	// Constructor.
	response(const boost_tcp::endpoint& sender_ep)
	{
		m_sender_addr = sender_ep.address().to_string();
		m_sender_port = sender_ep.port();
	}

	// Virtual destructor due to this class being an interface.
	virtual ~response() {}
	
	// Gets the sender address and port.
	std::tuple<std::string, unsigned short> get_sender() const
	{
		return { m_sender_addr, m_sender_port };
	}

	inline void set_sender(
		const std::string& sender_addr, 
		const unsigned short sender_port)
	{
		m_sender_addr = sender_addr;
		m_sender_port = sender_port;
	}

	// Checks if the given endpoint has the same address and port as the 
	// sender.
	bool is_sender(const boost_tcp::endpoint& ep)
	{
		return (ep.address().to_string() == m_sender_addr) and
			(ep.port() == m_sender_port);
	}

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<base_action>(*this);
		ar & m_sender_addr;
		ar & m_sender_port;
	}

private:
	std::string m_sender_addr;
	unsigned short m_sender_port;
};

}
};