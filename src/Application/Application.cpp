#include "Application.h"

#include <raylib.h>

#include "Logging/Utils.h"
#include "Network/Client.h"
#include "Network/Server.h"
#include "Network/Utils.h"

namespace fc
{

constexpr int DEFAULT_WINDOW_WIDTH{800};
constexpr int DEFAULT_WINDOW_HEIGHT{600};

int Application::Run(fc::Environment::Options& options, std::vector<Module>& modules)
{
    fc::Logging::Initialise();

    spdlog::info("Registering components...");
    for (auto module : modules)
    {
        module.RegisterComponents(mWorld);
    }

    int status;
    if (options.IsServer() || options.IsClient())
    {
        fc::Network::InitSteamDatagramConnectionSockets();

        if (options.IsServer())
        {
            status = RunAsServer(options, modules);
        }
        else if (options.IsClient())
        {
            status = RunAsClient(options, modules);
        }

        fc::Network::ShutdownSteamDatagramConnectionSockets();
    }
    else
    {
        status = RunAsMonolith(options, modules);
    }

    return status;
}

int Application::RunAsServer(fc::Environment::Options& options, std::vector<Module>& modules)
{
    // TODO: Implement basic CLI commands to do basic server ops.
    // TODO: Network stuff should probably run in a separate thread.
    fc::Network::Server server;

    if (!server.Start(options.GetListenPort()))
    {
        return -1;
    }

    spdlog::info("Initialising ECS...");
    for (auto module : modules)
    {
        module.InitCommonECS(mWorld);
        module.InitServerECS(mWorld);
    }

    spdlog::info("Entering main loop...");
    while (!mShouldQuit)
    {
        server.Poll();
        mWorld.progress();
    }

    server.Stop();

    return 0;
}

int Application::RunAsClient(fc::Environment::Options& options, std::vector<Module>& modules)
{
    fc::Network::Client client;

    InitWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "Flecs City");
    SetTargetFPS(60);

    SteamNetworkingIPAddr connectAddress;
    connectAddress.Clear();
    if (!connectAddress.ParseString(options.GetConnectAddress().c_str()))
    {
        spdlog::error("Failed to connect to {}", options.GetConnectAddress());
        return -1;
    }

    if (!client.Start(connectAddress))
        return -1;

    spdlog::info("Initialising ECS...");
    for (auto module : modules)
    {
        module.InitCommonECS(mWorld);
        module.InitClientECS(mWorld);
    }

    spdlog::info("Entering main loop...");
    while (!WindowShouldClose())
    {
        client.Poll();
        mWorld.progress();
    }

    CloseWindow();

    return 0;
}

int Application::RunAsMonolith(fc::Environment::Options& options, std::vector<Module>& modules)
{
    InitWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "Flecs City");
    SetTargetFPS(60);

    spdlog::info("Initialising ECS...");
    for (auto module : modules)
    {
        module.InitCommonECS(mWorld);
        module.InitServerECS(mWorld);
        module.InitClientECS(mWorld);
    }

    spdlog::info("Entering main loop...");
    while (!WindowShouldClose())
    {
        mWorld.progress();
    }

    CloseWindow();

    return 0;
}

}  // namespace fc
