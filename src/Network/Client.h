#pragma once

#include <steam/isteamnetworkingsockets.h>
#include <steam/steamnetworkingtypes.h>

namespace fc::Network
{

class Client
{
public:
    bool Start(const SteamNetworkingIPAddr& serverAddress);
    void Poll();
    void Stop();

private:
    HSteamNetConnection mConnection;
    ISteamNetworkingSockets* mInterface;

    void PollIncomingMessages();

    void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* info);

    static Client* sCallbackInstance;
    static void SteamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* info)
    {
        sCallbackInstance->OnSteamNetConnectionStatusChanged(info);
    }

    void PollConnectionStateChanges();
};

} // namespace fc::Network
