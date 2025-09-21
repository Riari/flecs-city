#pragma once

#include <cstdint>

#include <enet/enet.h>

#include "Network/NetworkThread.h"

namespace fc::Network
{

constexpr int TIMEOUT_CONNECT = 5000;
constexpr int TIMEOUT_DISCONNECT = 3000;

class ClientThread : public NetworkThread
{
public:
    bool Connect(const char* address, uint32_t port);
    void Disconnect() override;

protected:
    bool Init() override;
    void HandleEvent(const ENetEvent& event) override;

private:
    ENetPeer* mPeer{nullptr};
};

} // namespace fc::Network
