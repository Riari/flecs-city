#pragma once

#include <enet/enet.h>

namespace fc::Network
{

class Client
{
public:
    Client();
    ~Client();

    bool Connect(const char* address, uint32_t port);

private:
    ENetHost* mHost;
    ENetAddress mConnectionAddress;
    ENetPeer* mPeer;
};

} // namespace fc::Network
