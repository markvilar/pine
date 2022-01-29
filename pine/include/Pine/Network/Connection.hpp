#pragma once

#include <asio.hpp>

#include "Pine/Network/Message.hpp"
#include "Pine/Network/ThreadSafeQueue.hpp"

namespace Pine
{

template <typename T>
class Connection : public std::enable_shared_from_this<Connection<T>>
{
    using tcp = asio::ip::tcp;

public:
    enum class Owner
    {
        Server,
        Client
    };

    Connection(Owner owner, asio::io_context& context, tcp::socket socket,
        ThreadSafeQueue<OwnedMessage<T>>& messagesIn)
        : m_Owner(owner), m_Context(context), m_Socket(std::move(socket)),
          m_MessagesIn(messagesIn)
    {
    }

    virtual ~Connection() = default;

    uint32_t GetID() const { return m_ID; }

    void ConnectToClient(const uint32_t id = 0)
    {
        if (m_Owner == Owner::Server)
        {
            if (m_Socket.is_open())
            {
                m_ID = id;
                ReadHeader();
            }
        }
    }

    void ConnectToServer(const tcp::resolver::results_type& endpoints)
    {
        if (m_Owner == Owner::Client)
        {
            // TODO: Set up timeout.
            asio::async_connect(m_Socket,
                endpoints,
                [this](const std::error_code error,
                    const tcp::endpoint endpoint) {
                    if (error)
                    {
                        PINE_CORE_ERROR("[{0}] Connect to server failed: {1}",
                            m_ID,
                            error.message());
                        return;
                    }

                    ReadHeader();
                });
        }
    }

    void Disconnect()
    {
        if (IsConnected())
        {
            asio::post(m_Context, [this]() { m_Socket.close(); });
        }
    }

    bool IsConnected() const { return m_Socket.is_open(); }

public:
    void Send(const Message<T>& message)
    {
        asio::post(m_Context, [this, message]() {
            const auto writingMessages = !m_MessagesOut.empty();
            m_MessagesOut.push_back(message);
            if (!writingMessages)
            {
                WriteHeader();
            }
        });
    }

private:
    void ReadHeader()
    {
        auto message = std::make_shared<Message<T>>();

        asio::async_read(m_Socket,
            asio::buffer(&message->Header, sizeof(MessageHeader<T>)),
            [this, message](const std::error_code error,
                const uint64_t length) {
                if (error)
                {
                    PINE_CORE_ERROR("[{0}] Reading message header failed: {1}",
                        m_ID,
                        error.message());
                    m_Socket.close();
                    return;
                }

                if (message->Header.Size > 0)
                {
                    message->Body.resize(message->Header.Size);
                    ReadBody(message);
                }
                else
                {
                    AddToIncomingMessageQueue(message);
                }
            });
    }

    void ReadBody(std::shared_ptr<Message<T>> message)
    {
        asio::async_read(m_Socket,
            asio::buffer(message->Body.data(), message->Body.size()),
            [this, message](const std::error_code error,
                const uint64_t length) {
                if (error)
                {
                    PINE_CORE_ERROR("[{0}] Reading message body failed: {1}",
                        m_ID,
                        error.message());
                    m_Socket.close();
                    return;
                }

                AddToIncomingMessageQueue(message);
            });
    }

    void WriteHeader()
    {
        asio::async_write(m_Socket,
            asio::buffer(&m_MessagesOut.front().Header,
                sizeof(MessageHeader<T>)),
            [this](const std::error_code error, const uint64_t length) {
                if (error)
                {
                    PINE_CORE_ERROR("[{0}] Writing message header failed: {1}",
                        m_ID,
                        error.message());
                    m_Socket.close();
                    return;
                }

                const auto hasBody = m_MessagesOut.front().Body.size() > 0;
                if (hasBody)
                {
                    WriteBody();
                }
                else
                {
                    m_MessagesOut.pop_front();
                    if (!m_MessagesOut.empty())
                    {
                        WriteHeader();
                    }
                }
            });
    }

    void WriteBody()
    {
        asio::async_write(m_Socket,
            asio::buffer(m_MessagesOut.front().Body.data(),
                m_MessagesOut.front().Body.size()),
            [this](const std::error_code error, const uint64_t length) {
                if (error)
                {
                    PINE_CORE_ERROR("[{0}] Writing message body failed: {1}",
                        m_ID,
                        error.message());
                    m_Socket.close();
                    return;
                }

                m_MessagesOut.pop_front();
                if (!m_MessagesOut.empty())
                {
                    WriteHeader();
                }
            });
    }

    void AddToIncomingMessageQueue(const std::shared_ptr<Message<T>> message)
    {
        if (m_Owner == Owner::Server && message)
        {
            m_MessagesIn.push_back(
                {this->shared_from_this(), std::move(*message)});
        }
        else if (message)
        {
            m_MessagesIn.push_back({nullptr, std::move(*message)});
        }

        ReadHeader();
    }

protected:
    tcp::socket m_Socket;
    asio::io_context& m_Context;

    Owner m_Owner = Owner::Server;
    uint32_t m_ID = 0;

    ThreadSafeQueue<Message<T>> m_MessagesOut;
    ThreadSafeQueue<OwnedMessage<T>>& m_MessagesIn;
};

} // namespace Pine