#pragma once

#include <limits>
#include <memory>

#include "pine/core/common.hpp"
#include "pine/network/connection.hpp"
#include "pine/network/types.hpp"
#include "pine/utils/locked_queue.hpp"

namespace pine
{

struct ServerState
{
    // TODO: Replace with unique pointer.
    using Connection = ConnectionState;
    using ConnectionPtr = std::shared_ptr<ConnectionState>;

    NetworkContext context{};
    std::thread context_thread{};
    AcceptorType acceptor;

    std::deque<ConnectionPtr> connections{};
    LockedQueue<std::vector<uint8_t>> message_queue{};

public:
    ServerState(const uint16_t port);

    ServerState(const ServerState&) = delete;
    ServerState(ServerState&&) = delete;

    ServerState& operator=(const ServerState&) = delete;
    ServerState& operator=(ServerState&&) = delete;

    ~ServerState();
};

void stop_server(ServerState& server);
void send_to_client(ServerState& server,
    const std::shared_ptr<ConnectionState>& client, const uint8_t* data,
    const uint64_t size);

template <typename ConnectHandler>
void listen_for_clients(ServerState& server, ConnectHandler& handler)
{
    server.acceptor.async_accept(
        [&server, &handler](const std::error_code ec, SocketType socket)
        {
            if (!ec)
            {
                auto client = std::make_shared<ConnectionState>(server.context,
                    std::move(socket),
                    server.message_queue);

                if (handler(*client.get()))
                {
                    server.connections.push_back(client);
                    connect_to_client(*client.get());
                }
            }
            else
            {
                PINE_CORE_ERROR("Server error: {0}", ec.message());
            }

            listen_for_clients(server, handler);
        });
}

template <typename ConnectHandler>
bool start_server(ServerState& server, ConnectHandler handler)
{
    // TODO: Add static assert of connect handler signature.
    try
    {
        listen_for_clients(server, handler);
        server.context_thread =
            std::thread([&server]() { server.context.run(); });
    }
    catch (const std::exception& error)
    {
        PINE_CORE_ERROR("Server exception: {0}", error.what());
        return false;
    }
    return true;
}

template <typename MessageHandler>
void update_server(ServerState& server, MessageHandler handler,
    const uint64_t max_messages = std::numeric_limits<uint64_t>::max())
{
    // TODO: Add static assert of message handler signature.
    uint64_t message_count = 0;
    while (message_count < max_messages && !server.message_queue.empty())
    {
        const auto message = server.message_queue.pop_front();
        handler(message);
        message_count++;
    }
}

} // namespace pine
