#include "Server.h"

#include <cstdint>

#include <spdlog/spdlog.h>

namespace fc::Network
{

Server::Server(uint32_t listenPort)
{
    mAddress.host = ENET_HOST_ANY;
    mAddress.port = listenPort;

    mHost = enet_host_create(&mAddress, 32, 2, 0, 0);

    if (mHost == NULL)
    {
        spdlog::error("An error occurred while trying to create an ENet server host.");
        exit(EXIT_FAILURE);
    }
}

Server::~Server()
{
    enet_host_destroy(mHost);
}

} // namespace fc::Network
