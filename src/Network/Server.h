#pragma once

#include <enet/enet.h>

namespace fc::Network
{

class Server
{
public:
    Server(uint32_t listenPort);
    ~Server();

private:
    ENetAddress mAddress;
    ENetHost* mHost;
};

} // namespace fc::Network
