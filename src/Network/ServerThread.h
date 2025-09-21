#pragma once

#include <cstdint>
#include <vector>

#include <enet/enet.h>

#include "Network/NetworkThread.h"

namespace fc::Network
{

class ServerThread : public NetworkThread
{
public:
    ServerThread(uint32_t listenPort);

protected:
    bool Init() override;
    void HandleEvent(const ENetEvent& event) override;

private:
    std::vector<ENetPeer*> mConnectedPeers;
};

} // namespace fc::Network
