#include "Application.h"

#include <enet/enet.h>
#include <raylib.h>

#include "Logging/Utils.h"
#include "Network/ClientThread.h"
#include "Network/ServerThread.h"

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
        if (options.IsServer())
        {
            status = RunAsServer(options, modules);
        }
        else if (options.IsClient())
        {
            status = RunAsClient(options, modules);
        }
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
    fc::Network::ServerThread serverThread(options.GetListenPort());
    serverThread.Start();

    spdlog::info("Initialising ECS...");
    for (auto module : modules)
    {
        module.InitCommonECS(mWorld);
        module.InitServerECS(mWorld);
    }

    spdlog::info("Entering main loop...");
    while (!mShouldQuit)
    {
        mWorld.progress();
    }

    return 0;
}

int Application::RunAsClient(fc::Environment::Options& options, std::vector<Module>& modules)
{
    fc::Network::ClientThread clientThread;
    clientThread.Start();

    InitWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "Flecs City");
    SetTargetFPS(60);

    Environment::ConnectAddress connectAddress = options.GetConnectAddress();
    if (!clientThread.Connect(connectAddress.mHost, connectAddress.mPort))
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
