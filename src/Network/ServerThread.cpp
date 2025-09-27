#include "ServerThread.h"

#include <algorithm>
#include <cstdint>

#include <spdlog/spdlog.h>

namespace fc::Network
{

ServerThread::ServerThread(uint32_t listenPort)
    : NetworkThread()
{
    mAddress.host = ENET_HOST_ANY;
    mAddress.port = listenPort;
}

bool ServerThread::Init()
{
    mHost = enet_host_create(&mAddress, 32, 2, 0, 0);
    if (mHost == NULL)
    {
        spdlog::error("An error occurred while trying to create an ENet server host.");
        return false;
    }

    spdlog::info("Server host created.");
    mState = State::Active;

    return true;
}

void ServerThread::HandleEvent(const ENetEvent& event)
{
    NetworkThread::HandleEvent(event);

    switch (event.type)
    {
        case ENET_EVENT_TYPE_CONNECT:
            {
                char hostStr[64];
                enet_address_get_host_ip(&event.peer->address, hostStr, sizeof(hostStr));

                mConnectedPeers.push_back(event.peer);

                spdlog::info("Client connected from {}:{} (peer ID: {}). Total clients: {}.", hostStr, event.peer->address.port, event.peer->incomingPeerID, mConnectedPeers.size());
            }
            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            {
                char hostStr[64];
                enet_address_get_host_ip(&event.peer->address, hostStr, sizeof(hostStr));

                auto it = std::find(mConnectedPeers.begin(), mConnectedPeers.end(), event.peer);
                if (it != mConnectedPeers.end())
                {
                    mConnectedPeers.erase(it);
                }

                spdlog::info("Client disconnected from {}:{} (peer ID: {}). Remaining clients: {}.", hostStr, event.peer->address.port, event.peer->incomingPeerID, mConnectedPeers.size());
            }
            break;

        default:
            break;
    }
}

} // namespace fc::Network
