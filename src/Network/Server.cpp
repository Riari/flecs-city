#include "Server.h"

#include <chrono>

#include <spdlog/spdlog.h>
#include <steam/isteamnetworkingsockets.h>
#include <steam/steamnetworkingtypes.h>

namespace fc::Network
{

bool Server::Start(uint16 port)
{
    mInterface = SteamNetworkingSockets();

    SteamNetworkingIPAddr serverLocalAddress;
    serverLocalAddress.Clear();
    serverLocalAddress.m_port = port;
    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)SteamNetConnectionStatusChangedCallback);

    mListenSocket = mInterface->CreateListenSocketIP(serverLocalAddress, 1, &opt);
    if (mListenSocket == k_HSteamListenSocket_Invalid)
    {
        spdlog::error("Failed to listen on port {}", port);
        return false;
    }

    mPollGroup = mInterface->CreatePollGroup();
    if (mPollGroup == k_HSteamNetPollGroup_Invalid)
    {
        spdlog::error("Failed to listen on port {}", port);
        return false;
    }

    spdlog::info("Server listening on port {}", port);
    return true;
}

void Server::Poll()
{
    PollIncomingMessages();
    PollConnectionStateChanges();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void Server::Stop()
{
    spdlog::info("Closing connections...");
    for (auto it : mMapClients)
    {
        SendStringToClient(it.first, "Server is shutting down. Goodbye.");

        mInterface->CloseConnection(it.first, 0, "Server Shutdown", true);
    }

    mMapClients.clear();

    mInterface->CloseListenSocket(mListenSocket);
    mListenSocket = k_HSteamListenSocket_Invalid;

    mInterface->DestroyPollGroup(mPollGroup);
    mPollGroup = k_HSteamNetPollGroup_Invalid;
}

void Server::SendStringToClient(HSteamNetConnection connection, const char* str)
{
    mInterface->SendMessageToConnection(connection, str, (uint32)strlen(str), k_nSteamNetworkingSend_Reliable, nullptr);
}

void Server::SendStringToAllClients(const char* str, HSteamNetConnection except)
{
    for (auto& client : mMapClients)
    {
        if (client.first != except)
            SendStringToClient(client.first, str);
    }
}

void Server::PollIncomingMessages()
{
    char buffer[1024];

    while (true)
    {
        ISteamNetworkingMessage* messages = nullptr;
        int messageCount = mInterface->ReceiveMessagesOnPollGroup(mPollGroup, &messages, 1);

        if (messageCount == 0)
            break;

        if (messageCount < 0)
        {
            spdlog::error("An error occurred while polling for incoming messages");
            return;
        }

        assert(messageCount == 1 && messages);

        auto client = mMapClients.find(messages->m_conn);

        std::string messageString;
        messageString.assign((const char*)messages->m_pData, messages->m_cbSize);
        const char* message = messageString.c_str();

        messages->Release();

        if (strncmp(message, "/nick", 5) == 0)
        {
            const char* nick = message + 5;
            while (isspace(*nick))
                ++nick;

            sprintf(buffer, "%s shall henceforth be known as %s", client->second.mNick.c_str(), nick);
            SendStringToAllClients(buffer, client->first);

            sprintf(buffer, "Ye shall henceforth be known as %s", nick);
            SendStringToClient(client->first, buffer);

            SetClientNick(client->first, nick);

            continue;
        }

        sprintf(buffer, "%s: %s", client->second.mNick.c_str(), message);
        SendStringToAllClients(buffer, client->first);
    }
}

void Server::SetClientNick(HSteamNetConnection connection, const char* nick)
{
    mMapClients[connection].mNick = nick;

    mInterface->SetConnectionName(connection, nick);
}

void Server::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* info)
{
    char buffer[1024];

    switch (info->m_info.m_eState)
    {
        case k_ESteamNetworkingConnectionState_None:
            // NOTE: We will get callbacks here when we destroy connections.  You can ignore these.
            break;

        case k_ESteamNetworkingConnectionState_ClosedByPeer:
        case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
        {
            // Ignore if they were not previously connected.  (If they disconnected
            // before we accepted the connection.)
            if (info->m_eOldState == k_ESteamNetworkingConnectionState_Connected)
            {
                // Locate the client.  Note that it should have been found, because this
                // is the only codepath where we remove clients (except on shutdown),
                // and connection change callbacks are dispatched in queue order.
                auto itClient = mMapClients.find(info->m_hConn);
                assert(itClient != mMapClients.end());

                // Select appropriate log messages
                const char* pszDebugLogAction;
                if (info->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
                {
                    pszDebugLogAction = "problem detected locally";
                    sprintf(buffer, "Alas, %s hath fallen into shadow.  (%s)", itClient->second.mNick.c_str(), info->m_info.m_szEndDebug);
                }
                else
                {
                    // Note that here we could check the reason code to see if
                    // it was a "usual" connection or an "unusual" one.
                    pszDebugLogAction = "closed by peer";
                    sprintf(buffer, "%s hath departed", itClient->second.mNick.c_str());
                }

                // Spew something to our own log.  Note that because we put their nick
                // as the connection description, it will show up, along with their
                // transport-specific data (e.g. their IP address)
                spdlog::info("Connection {} {}, reason {}: {}", info->m_info.m_szConnectionDescription, pszDebugLogAction, info->m_info.m_eEndReason, info->m_info.m_szEndDebug);

                mMapClients.erase(itClient);

                // Send a message so everybody else knows what happened
                SendStringToAllClients(buffer);
            }
            else
            {
                assert(info->m_eOldState == k_ESteamNetworkingConnectionState_Connecting);
            }

            // Clean up the connection.  This is important!
            // The connection is "closed" in the network sense, but
            // it has not been destroyed.  We must close it on our end, too
            // to finish up.  The reason information do not matter in this case,
            // and we cannot linger because it's already closed on the other end,
            // so we just pass 0's.
            mInterface->CloseConnection(info->m_hConn, 0, nullptr, false);
            break;
        }

        case k_ESteamNetworkingConnectionState_Connecting:
        {
            // This must be a new connection
            assert(mMapClients.find(info->m_hConn) == mMapClients.end());

            spdlog::info("Connection request from {}", info->m_info.m_szConnectionDescription);

            // A client is atbufferting to connect
            // Try to accept the connection.
            if (mInterface->AcceptConnection(info->m_hConn) != k_EResultOK)
            {
                // This could fail.  If the remote host tried to connect, but then
                // disconnected, the connection may already be half closed.  Just
                // destroy whatever we have on our side.
                mInterface->CloseConnection(info->m_hConn, 0, nullptr, false);
                spdlog::error("Can't accept connection. (It was already closed?)");
                break;
            }

            // Assign the poll group
            if (!mInterface->SetConnectionPollGroup(info->m_hConn, mPollGroup))
            {
                mInterface->CloseConnection(info->m_hConn, 0, nullptr, false);
                spdlog::error("Failed to set poll group?");
                break;
            }

            // Generate a random nick.  A random bufferorary nick
            // is really dumb and not how you would write a real chat server.
            // You would want them to have some sort of signon message,
            // and you would keep their client in a state of limbo (connected,
            // but not logged on) until them.  I'm trying to keep this example
            // code really simple.
            char nick[64];
            sprintf(nick, "BraveWarrior%d", 10000 + (rand() % 100000));

            // Send them a welcome message
            sprintf(buffer, "Welcome, stranger.  Thou art known to us for now as '%s'; upon thine command '/nick' we shall know thee otherwise.", nick);
            SendStringToClient(info->m_hConn, buffer);

            // Also send them a list of everybody who is already connected
            if (mMapClients.empty())
            {
                SendStringToClient(info->m_hConn, "Thou art utterly alone.");
            }
            else
            {
                sprintf(buffer, "%d companions greet you:", (int)mMapClients.size());
                for (auto& c : mMapClients)
                    SendStringToClient(info->m_hConn, c.second.mNick.c_str());
            }

            // Let everybody else know who they are for now
            sprintf(buffer, "Hark!  A stranger hath joined this merry host.  For now we shall call them '%s'", nick);
            SendStringToAllClients(buffer, info->m_hConn);

            // Add them to the client list, using std::map wacky syntax
            mMapClients[info->m_hConn];
            SetClientNick(info->m_hConn, nick);
            break;
        }

        case k_ESteamNetworkingConnectionState_Connected:
            // We will get a callback immediately after accepting the connection.
            // Since we are the server, we can ignore this, it's not news to us.
            break;

        default:
            // Silences -Wswitch
            break;
    }
}

void Server::PollConnectionStateChanges()
{
    sCallbackInstance = this;
    mInterface->RunCallbacks();
}

Server* Server::sCallbackInstance = nullptr;

}  // namespace fc::Network
