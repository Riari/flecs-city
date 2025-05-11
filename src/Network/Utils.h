#pragma once

#include <spdlog/spdlog.h>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>

namespace fc::Network
{

static SteamNetworkingMicroseconds gLogTimeZero;

inline void DebugOutput(ESteamNetworkingSocketsDebugOutputType eType, const char* pszMsg)
{
    SteamNetworkingMicroseconds time = SteamNetworkingUtils()->GetLocalTimestamp() - gLogTimeZero;
    spdlog::debug("{0:0.6f} {1}", time * 1e-6, pszMsg);
    fflush(stdout);
    if (eType == k_ESteamNetworkingSocketsDebugOutputType_Bug)
    {
        fflush(stdout);
        fflush(stderr);
    }
}

inline bool InitSteamDatagramConnectionSockets()
{
    SteamDatagramErrMsg error;
    if (!GameNetworkingSockets_Init(nullptr, error))
    {
        spdlog::error("GameNetworkingSockets_Init failed. {}", error);
        return false;
    }

    gLogTimeZero = SteamNetworkingUtils()->GetLocalTimestamp();

    SteamNetworkingUtils()->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Msg, DebugOutput);

    return true;
}

inline void ShutdownSteamDatagramConnectionSockets()
{
    // Give connections time to finish up.  This is an application layer protocol
    // here, it's not TCP.  Note that if you have an application and you need to be
    // more sure about cleanup, you won't be able to do this.  You will need to send
    // a message and then either wait for the peer to close the connection, or
    // you can pool the connection to see if any reliable data is pending.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    GameNetworkingSockets_Kill();
}

}  // namespace fc::Network
