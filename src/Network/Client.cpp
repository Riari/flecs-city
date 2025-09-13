#include "Client.h"

#include <cstdlib>

#include <spdlog/spdlog.h>

namespace fc::Network
{

Client::Client()
{
    mHost = enet_host_create(NULL, 1, 2, 0, 0);
    if (mHost == NULL)
    {
        spdlog::error("An error occurred while trying to create an ENet client host.");
        exit(EXIT_FAILURE);
    }
}

Client::~Client()
{
    enet_host_destroy(mHost);
}


bool Client::Connect(const char* address, uint32_t port)
{
    enet_address_set_host(&mConnectionAddress, address);
    mConnectionAddress.port = port;

    mPeer = enet_host_connect(mHost, &mConnectionAddress, 2, 0);
    if (mPeer == NULL)
    {
        spdlog::error("No available peers for initiating an ENet connection.");
        return false;
    }

    ENetEvent event;
    if (enet_host_service(mHost, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        spdlog::info("Connection to {}:{} succeeded.", address, port);
        return true;
    }

    enet_peer_reset(mPeer);
    spdlog::error("Connection to {}:{} failed.", address, port);
    return false;
}

} // namespace fc::Network
