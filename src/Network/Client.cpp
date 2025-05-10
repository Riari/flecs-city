#include "Client.h"

#include <chrono>

#include <spdlog/spdlog.h>
#include <steam/isteamnetworkingutils.h>

namespace fc::Network
{

bool Client::Start(const SteamNetworkingIPAddr& serverAddress)
{
    mInterface = SteamNetworkingSockets();

    char address[SteamNetworkingIPAddr::k_cchMaxString];
    serverAddress.ToString(address, sizeof(address), true);

    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)SteamNetConnectionStatusChangedCallback);

    spdlog::info("Connecting to server at {}...", address);
    mConnection = mInterface->ConnectByIPAddress(serverAddress, 1, &opt);
    if (mConnection == k_HSteamNetConnection_Invalid)
    {
        spdlog::error("Failed to connect");
        return false;
    }

    return true;
}

void Client::Poll()
{
    PollIncomingMessages();
    PollConnectionStateChanges();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void Client::Stop()
{
    mInterface->CloseConnection(mConnection, 0, "Goodbye", true);
}

void Client::PollIncomingMessages()
{
    while (true)
    {
        ISteamNetworkingMessage* messages = nullptr;
        int messageCount = mInterface->ReceiveMessagesOnConnection(mConnection, &messages, 1);

        if (messageCount == 0)
            break;

        if (messageCount < 0)
        {
            spdlog::error("Client: An error occurred while polling for incoming messages");
            break;
        }

        std::string messageString;
        messageString.assign((const char*)messages->m_pData, messages->m_cbSize);

        spdlog::debug("> {}", messageString);

        messages->Release();
    }
}

void Client::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* info)
{
    assert(info->m_hConn == mConnection || mConnection == k_HSteamNetConnection_Invalid);

    // What's the state of the connection?
    switch (info->m_info.m_eState)
    {
        case k_ESteamNetworkingConnectionState_None:
            // NOTE: We will get callbacks here when we destroy connections.  You can ignore these.
            break;

        case k_ESteamNetworkingConnectionState_ClosedByPeer:
        case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
        {
            // Print an appropriate message
            if (info->m_eOldState == k_ESteamNetworkingConnectionState_Connecting)
            {
                // Note: we could distinguish between a timeout, a rejected connection,
                // or some other transport problem.
                spdlog::debug("We sought the remote host, yet our efforts were met with defeat. {}", info->m_info.m_szEndDebug);
            }
            else if (info->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
            {
                spdlog::debug("Alas, troubles beset us; we have lost contact with the host. {}", info->m_info.m_szEndDebug);
            }
            else
            {
                // NOTE: We could check the reason code for a normal disconnection
                spdlog::debug("The host hath bidden us farewell. {}", info->m_info.m_szEndDebug);
            }

            // Clean up the connection.  This is important!
            // The connection is "closed" in the network sense, but
            // it has not been destroyed.  We must close it on our end, too
            // to finish up.  The reason information do not matter in this case,
            // and we cannot linger because it's already closed on the other end,
            // so we just pass 0's.
            mInterface->CloseConnection(info->m_hConn, 0, nullptr, false);
            mConnection = k_HSteamNetConnection_Invalid;
            break;
        }

        case k_ESteamNetworkingConnectionState_Connecting:
            // We will get this callback when we start connecting.
            // We can ignore this.
            break;

        case k_ESteamNetworkingConnectionState_Connected:
            spdlog::debug("Connected to server OK");
            break;

        default:
            // Silences -Wswitch
            break;
    }
}

void Client::PollConnectionStateChanges()
{
    sCallbackInstance = this;
    mInterface->RunCallbacks();
}

Client* Client::sCallbackInstance = nullptr;

}  // namespace fc::Network
