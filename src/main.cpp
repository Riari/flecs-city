#include <vector>

#include <args.hxx>
#include <flecs.h>
#include <raylib.h>
#include <spdlog/spdlog.h>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>

#include <Modules/Core/Core.h>

#include "Network/Server.h"

static bool InitArgs(args::ArgumentParser& parser, int argc, char** argv)
{
    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        std::cout << parser;
        return false;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return false;
    }
    catch (args::ValidationError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return false;
    }

    return true;
}

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

int main(int argc, char** argv)
{
    args::ArgumentParser parser("Flecs City");
    args::Flag listen(parser, "listen", "Start in dedicated server mode", {"listen"});

    InitArgs(parser, argc, argv);

    InitSteamDatagramConnectionSockets();

    fc::Network::Server server;
    if (listen)
    {
        server.Start(6969);
    }

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
        if (listen)
            server.Poll();

        ecs.progress();
    }

    CloseWindow();

    ShutdownSteamDatagramConnectionSockets();

    return 0;
}
