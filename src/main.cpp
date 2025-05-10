#include <vector>

#include <flecs.h>
#include <raylib.h>
#include <spdlog/spdlog.h>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>

#include <Modules/Core/Core.h>

#include "Environment/Options.h"
#include "Network/Client.h"
#include "Network/Server.h"

SteamNetworkingMicroseconds gLogTimeZero;

static void DebugOutput(ESteamNetworkingSocketsDebugOutputType eType, const char* pszMsg)
{
    SteamNetworkingMicroseconds time = SteamNetworkingUtils()->GetLocalTimestamp() - gLogTimeZero;
    spdlog::debug("%10.6f %s", time * 1e-6, pszMsg);
    fflush(stdout);
    if (eType == k_ESteamNetworkingSocketsDebugOutputType_Bug)
    {
        fflush(stdout);
        fflush(stderr);
    }
}

static bool InitSteamDatagramConnectionSockets()
{
    SteamDatagramErrMsg error;
    if (!GameNetworkingSockets_Init(nullptr, error))
    {
        spdlog::error("GameNetworkingSockets_Init failed. %s", error);
        return false;
    }

    gLogTimeZero = SteamNetworkingUtils()->GetLocalTimestamp();

    SteamNetworkingUtils()->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Msg, DebugOutput);

    return true;
}

static void ShutdownSteamDatagramConnectionSockets()
{
    // Give connections time to finish up.  This is an application layer protocol
    // here, it's not TCP.  Note that if you have an application and you need to be
    // more sure about cleanup, you won't be able to do this.  You will need to send
    // a message and then either wait for the peer to close the connection, or
    // you can pool the connection to see if any reliable data is pending.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    GameNetworkingSockets_Kill();
}

static void RaylibLog(int type, const char* text, va_list args)
{
    char buffer[2048];
    vsnprintf(buffer, sizeof(buffer), text, args);

    // Forward to spdlog with appropriate level
    switch (type)
    {
        case LOG_TRACE:
            spdlog::trace("{}", buffer);
            break;
        case LOG_DEBUG:
            spdlog::debug("{}", buffer);
            break;
        case LOG_INFO:
            spdlog::info("{}", buffer);
            break;
        case LOG_WARNING:
            spdlog::warn("{}", buffer);
            break;
        case LOG_ERROR:
            spdlog::error("{}", buffer);
            break;
        case LOG_FATAL:
            spdlog::critical("{}", buffer);
            break;
        default:
            break;
    }
}

int main(int argc, char** argv)
{
    fc::Environment::Options options;
    if (!options.Init(argc, argv))
        return;

    InitSteamDatagramConnectionSockets();

    fc::Network::Server server;
    fc::Network::Client client;

    if (options.IsServer())
    {
        if (!server.Start(6969))
            return -1;
    }
    else if (options.IsClient())
    {
        SteamNetworkingIPAddr connectAddress;
        connectAddress.Clear();
        if (!connectAddress.ParseString(options.GetConnectAddress().c_str()))
        {
            spdlog::error("Failed to connect to {}", options.GetConnectAddress());
            return -1;
        }

        if (!client.Start(connectAddress))
            return -1;
    }

    SetTraceLogCallback(RaylibLog);

    constexpr int width = 1920;
    constexpr int height = 1080;

    InitWindow(width, height, "Test window");

    SetTargetFPS(60);

    flecs::world ecs;

    std::vector<fc::Module> modules{
        fc::Core::MODULE,
    };

    for (auto module : modules)
    {
        module.RegisterComponents(ecs);
    }

    for (auto module : modules)
    {
        module.InitECS(ecs);
    }

    while (!WindowShouldClose())
    {
        if (options.IsServer())
            server.Poll();
        else if (options.IsClient())
            client.Poll();

        ecs.progress();
    }

    CloseWindow();

    if (options.IsServer())
        server.Stop();
    else if (options.IsClient())
        client.Stop();

    ShutdownSteamDatagramConnectionSockets();

    return 0;
}
