#pragma once

#include <cstdint>
#include <vector>

#include <enet/enet.h>

#include "Network/NetworkThread.h"
#include "Network/ReplicationPacket.h"

namespace fc::Network
{

class ServerThread : public NetworkThread
{
public:
    ServerThread(uint32_t listenPort);

    void QueueReplicationPacket(const ReplicationPacket& packet);

protected:
    bool Init() override;
    void HandleEvent(const ENetEvent& event) override;
    void Update() override;

private:
    std::vector<ENetPeer*> mConnectedPeers;

    std::mutex mReplicationMutex;
    std::queue<ReplicationPacket> mReplicationQueue;

    void ProcessReplicationQueue();
};

} // namespace fc::Network
