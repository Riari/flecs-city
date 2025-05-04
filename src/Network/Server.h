#pragma once

#include <map>
#include <string>

#include <steam/isteamnetworkingsockets.h>
#include <steam/steamnetworkingtypes.h>

namespace fc::Network
{

class Server
{
public:
    bool Start(uint16 port);
    void Poll();
    void Stop();

private:
    HSteamListenSocket mListenSocket;
    HSteamNetPollGroup mPollGroup;
    ISteamNetworkingSockets* mInterface;

    struct Client_t
    {
        std::string mNick;
    };

    std::map<HSteamNetConnection, Client_t> mMapClients;

    void SendStringToClient(HSteamNetConnection connection, const char* str);
    void SendStringToAllClients(const char* str, HSteamNetConnection except = k_HSteamNetConnection_Invalid);

    void PollIncomingMessages();

    void SetClientNick(HSteamNetConnection connection, const char* nick);

    void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* info);

    static Server* sCallbackInstance;
    static void SteamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* info)
    {
        sCallbackInstance->OnSteamNetConnectionStatusChanged(info);
    }

    void PollConnectionStateChanges();
};

} // namespace fc::Network
