#include <csignal>

#include "pine/pine.hpp"

static bool exitFlag = false;

void signalHandler(const int signum) { exitFlag = true; }

int main(int argc, char** argv)
{
    signal(SIGINT, signalHandler);

    pine::Log::init();

    pine::ServerState server(6000);

    pine::start_server(server,
        [](const pine::ConnectionState& connection) -> bool
        {
            PINE_INFO("Server got connection: {0}",
                connection.socket.remote_endpoint());
            return true;
        });

    while (!exitFlag)
    {
        pine::update_server(server,
            [](const std::vector<uint8_t>& message) -> void
            {
                PINE_INFO("Server got message: {0} : {1}",
                    message.size(),
                    std::string(message.begin(), message.end()));
            });
    }

    PINE_INFO("Stopping server.");
    pine::stop_server(server);

    return 0;
}
