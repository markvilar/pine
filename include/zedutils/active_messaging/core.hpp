#ifndef AM_CORE_HPP
#define AM_CORE_HPP

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/asio.hpp>
#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/cstdint.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/ref.hpp>

#include <zedutils/active_messaging/container_device.hpp>

namespace zed { namespace am {

// Boost documentation references:
// https://www.boost.org/doc/libs/1_66_0/doc/html/boost_asio/reference.html
// https://www.boost.org/doc/libs/1_67_0/libs/assert/doc/html/assert.html

// Forward declaration
class runtime;

// ---------------------------------- action -----------------------------------

class action 
{
private:

public:
	virtual ~action() {}
	
	virtual void operator()(runtime& rt) = 0;

	virtual action* clone() const = 0;

	// Member function needed in order to use Boost.Serialization.
	template<typename Archive>
	void serialize(Archive& ar, const unsigned int version) {};
}; // class action


// -------------------------------- connection ---------------------------------

class connection : public std::enable_shared_from_this<connection>
{
private:
	// Reference to the Runtime object.
	runtime& m_runtime;

	// IP/TCP socket of the connection.
	boost::asio::ip::tcp::socket m_socket;

	// Size of the incoming parcel.
	boost::uint64_t m_in_size;

	// Buffer for incoming parcel.
	std::vector<char>* m_in_buffer;

public:
	connection(runtime& rt);
		
	~connection();

	// Returns the reference of the connection socket.
	boost::asio::ip::tcp::socket& get_socket() { return m_socket; }

	// Returns the remote endpoint of the connection socket.
	boost::asio::ip::tcp::endpoint get_remote_endpoint() const;

	// Asynchronously read a parcel from the socket.
	void async_read();

	// Asynchronously write an action to the socket.
	void async_write(action const& act);

	// Asynchronously write an action to the socket.
	void async_write(
		action const& act,
		std::function<void(boost::system::error_code const&)> handler
		);

	// Asynchronously write worker. It performs the action serializing.
	void async_write_worker(
		std::shared_ptr<action> act,
		std::function<void(boost::system::error_code const&)> handler
		);

	// Handler for parcel size.
	void handle_read_size(boost::system::error_code const& error);

	// Handler for data.
	void handle_read_data(boost::system::error_code const& error);

	// Handler for write.
	void handle_write(
		boost::system::error_code const& error,
		std::shared_ptr<boost::uint64_t> out_size,
		std::shared_ptr<std::vector<char>> out_buffer,
		std::function<void(boost::system::error_code const&)> handler
		);
}; // class connection


// ---------------------------------- runtime ----------------------------------

class runtime
{
private:
	// IO service.
	boost::asio::io_service m_io_service;

	// Connection acceptor.
	boost::asio::ip::tcp::acceptor m_acceptor;

	// Map with endpoints and connections.
	std::map<boost::asio::ip::tcp::endpoint, std::shared_ptr<connection>>
		m_connections;

	// Execution thread.
	std::thread m_exec_thread;

	// Parcel queue.
	boost::lockfree::queue<std::vector<char>*> m_parcel_queue;

	// Action queue.
	boost::lockfree::queue<std::function<void(runtime&)>*> m_local_queue;

	// Stop flag.
	std::atomic<bool> m_stop_flag;

	// Main function handler for bootstrapping.
	std::function<void(runtime&)> m_main;

	// The number of clients to wait for before executing the main function.
	boost::uint64_t m_wait_for;

public:
	runtime(
		std::string port,
		std::function<void(runtime&)> f 
			= std::function<void(runtime&)>(),
		boost::uint64_t wait_for = 1
		);
		
	~runtime() 
	{ 
		stop(); 
	}
	
	boost::asio::io_service& get_io_service() 
	{ 
		return m_io_service; 
	}

	boost::lockfree::queue<std::vector<char>*>& get_parcel_queue() 
	{ 
		return m_parcel_queue;
	}

	boost::lockfree::queue<std::function<void(runtime&)>*>&
		get_local_queue()
	{
		return m_local_queue;
	}

	std::map<boost::asio::ip::tcp::endpoint, std::shared_ptr<connection>>&
		get_connections()
	{
		return m_connections;
	}

	// Launch the execution thread and the start accepting connections.
	void start();

	// Stop the I/O service and execution thread.
	void stop();

	// Accepts connections and parcels until stop() is called.
	void run();

	// Connect to another node.
	std::shared_ptr<connection> connect(
		std::string host,
		std::string port
		);

	// Asynchronously accept a new connection.
	void async_accept();

	// Handler for new connections.
	void handle_accept(
		boost::system::error_code const& error,
		std::shared_ptr<connection> conn
		);

private:
	friend class connection;

	// Execute actions until stop() is called.
	void exec_loop();

	// Serializes an action object into a parcel.
	std::vector<char>* serialize_parcel(action const& act);

	// Deserializes a parcel into an action object.
	action* deserialize_parcel(std::vector<char>& raw_msg);
}; // class runtime

} // namespace am
}; // namespace zed

#endif // RUNTIME_HPP
