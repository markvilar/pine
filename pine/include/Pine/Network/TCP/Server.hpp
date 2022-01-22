#pragma once

#include <asio.hpp>

#include "Pine/Network/TCP/Connection.hpp"
#include "Pine/Network/TCP/Message.hpp"
#include "Pine/Network/ThreadSafeQueue.hpp"

namespace Pine::TCP
{

template <typename T> class Server
{
    // Server class interface.
public:
    Server(const uint16_t port)
        : m_Acceptor(
            m_Context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
    {
    }

    virtual ~Server() { Stop(); }

    bool Start()
    {
        try
        {
            WaitForClientConnection();

            m_ContextThread = std::thread([this]() { m_Context.run(); });
        }
        catch (std::exception& e)
        {
            PINE_CORE_ERROR("[Server] Exception: {0}", e.what());
            return false;
        }

        PINE_CORE_INFO("[Server] Started!");
        return true;
    }

    void Stop()
    {
        m_Context.stop();

        if (m_ContextThread.joinable())
        {
            m_ContextThread.join();
        }

        PINE_CORE_INFO("[Server] Stopped!");
    }

    // ASYNC - Instruct asio to wait for connection.
    void WaitForClientConnection()
    {
        m_Acceptor.async_accept(
            [this](std::error_code ec, asio::ip::tcp::socket socket) {
                if (!ec)
                {
                    PINE_CORE_INFO("[Server] New Connection: {0}",
                        socket.remote_endpoint());

                    auto newConnection = std::make_shared<Connection<T>>(
                        Connection<T>::Owner::Server,
                        m_Context,
                        std::move(socket),
                        m_MessagesIn);

                    if (OnClientConnect(newConnection))
                    {
                        m_Connections.push_back(newConnection);
                        m_Connections.back()->ConnectToClient(m_IDCounter++);
                        PINE_CORE_INFO("[Server] Connection {0} approved.",
                            m_Connections.back()->GetID());
                    }
                    else
                    {
                        PINE_CORE_INFO("[Server] Connection denied.");
                    }
                }
                else
                {
                    PINE_CORE_ERROR("[Server] New connection error: {0}",
                        ec.message());
                }

                WaitForClientConnection();
            });
    }

    void MessageClient(
        std::shared_ptr<Connection<T>> client, const Message<T>& message)
    {
        if (client && client->IsConnected())
        {
            client->Send(message);
        }
        else
        {
            OnClientDisconnect(client);
            client.reset();
            m_Connections.erase(
                std::remove(m_Connections.begin(), m_Connections.end(), client),
                m_Connections.end());
        }
    }

    void MessageAllClients(const Message<T>& message,
        std::shared_ptr<Connection<T>> ignoreClient = nullptr)
    {
        bool invalidClientExists = false;
        for (auto& client : m_Connections)
        {
            if (client && client->IsConnected())
            {
                if (client != ignoreClient)
                {
                    client->Send(message);
                }
            }
            else
            {
                OnClientDisconnect(client);
                client.reset();
                invalidClientExists = true;
            }
        }

        if (invalidClientExists)
        {
            m_Connections.erase(std::remove(m_Connections.begin(),
                                    m_Connections.end(),
                                    nullptr),
                m_Connections.end());
        }
    }

    void Update(uint64_t maxMessages = -1)
    {
        uint64_t messageCount = 0;
        while (messageCount < maxMessages && !m_MessagesIn.empty())
        {
            auto message = m_MessagesIn.pop_front();
            OnMessage(message.Remote, message.Msg);

            messageCount++;
        }
    }

protected:
    virtual bool OnClientConnect(std::shared_ptr<Connection<T>> client)
    {
        return false;
    }

    virtual void OnClientDisconnect(std::shared_ptr<Connection<T>> client) {}

    virtual void OnMessage(
        std::shared_ptr<Connection<T>> client, Message<T>& message)
    {
    }

protected:
    ThreadSafeQueue<OwnedMessage<T>> m_MessagesIn;

    asio::io_context m_Context;
    std::thread m_ContextThread;

    asio::ip::tcp::acceptor m_Acceptor;

    uint32_t m_IDCounter = 10000;
    std::deque<std::shared_ptr<Connection<T>>> m_Connections;
};

} // namespace Pine::TCP
