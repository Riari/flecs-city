#include "ClientThread.h"

#include <cstdlib>

#include <enet/enet.h>
#include <spdlog/spdlog.h>

namespace fc::Network
{

bool ClientThread::Connect(const char* address, uint32_t port)
{
    // TODO: Handle case where we're already connected

    enet_address_set_host(&mAddress, address);
    mAddress.port = port;

    mPeer = enet_host_connect(mHost, &mAddress, 2, 0);
    if (mPeer == NULL)
    {
        spdlog::error("No available peers for initiating an ENet connection.");
        return false;
    }

    ENetEvent event;
    if (enet_host_service(mHost, &event, TIMEOUT_CONNECT) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        spdlog::info("Connection to {}:{} succeeded.", address, port);
        mState = State::Active;
        return true;
    }

    enet_peer_reset(mPeer);
    spdlog::error("Connection to {}:{} failed.", address, port);
    return false;
}

void ClientThread::Disconnect()
{
    if (!mPeer || mPeer->state == ENET_PEER_STATE_DISCONNECTED)
        return;

    enet_peer_disconnect(mPeer, 0);

    ENetEvent event;
    while (enet_host_service(mHost, &event, TIMEOUT_DISCONNECT) > 0)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                spdlog::info("Disconnected.");
                mState = State::Idle;
                return;

            default:
                break;
        }
    }

    // Timed out waiting to disconnect. Force the connection down.
    enet_peer_reset(mPeer);
    mState = State::Idle;
}

bool ClientThread::Init()
{
    mHost = enet_host_create(NULL, 1, 2, 0, 0);
    if (mHost == NULL)
    {
        spdlog::error("An error occurred while trying to create an ENet client host.");
        return false;
    }

    spdlog::info("Client host created.");

    return true;
}

void ClientThread::HandleEvent(const ENetEvent& event)
{
    NetworkThread::HandleEvent(event);

    switch (event.type)
    {
        case ENET_EVENT_TYPE_DISCONNECT:
            spdlog::info("Disconnected");
            break;

        default:
            break;
    }
}

} // namespace fc::Network
