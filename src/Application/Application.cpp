#include "Application.h"

#include <enet/enet.h>
#include <raylib.h>

#include "Logging/Utils.h"
#include "Network/ClientThread.h"
#include "Network/ReplicationPacket.h"

namespace fc
{

constexpr int DEFAULT_WINDOW_WIDTH{800};
constexpr int DEFAULT_WINDOW_HEIGHT{600};

Application::Application()
{
    mComponentRegistry = new ECS::ComponentRegistry(mEcs);
}

Application::~Application()
{
    delete mComponentRegistry;
}

int Application::Run(fc::Environment::Options& options, std::vector<Module>& modules)
{
    fc::Logging::Initialise();

    spdlog::info("Registering components...");
    for (auto module : modules)
    {
        module.RegisterComponents(mComponentRegistry);
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

    for (auto module : modules)
    {
        module.Cleanup(mEcs);
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
        module.InitCommonECS(mEcs);
        module.InitServerECS(mEcs);
    }

    spdlog::info("Entering main loop...");
    while (!mShouldQuit)
    {
        mEcs.progress();
        UpdateReplication(serverThread);
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
        module.InitCommonECS(mEcs);
        module.InitClientECS(mEcs);
    }

    spdlog::info("Entering main loop...");
    while (!WindowShouldClose())
    {
        mEcs.progress();
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
        module.InitCommonECS(mEcs);
        module.InitServerECS(mEcs);
        module.InitClientECS(mEcs);
    }

    spdlog::info("Entering main loop...");
    while (!WindowShouldClose())
    {
        mEcs.progress();
    }

    CloseWindow();

    return 0;
}

void Application::UpdateReplication(fc::Network::ServerThread& serverThread)
{
    mEcs.each<ReplicatedComponent>([&](flecs::entity e, ReplicatedComponent& rep)
    {
        if (!rep.mIsDirty) return;

        Network::ReplicationPacket packet;
        packet.mEntityId = e.id();
        packet.mIsNewEntity = rep.mIsNewEntity;

        for (size_t i = 0; i < rep.mDirtyComponentCount; ++i)
        {
            flecs::id_t componentId = rep.mDirtyComponents[i];

            auto schema = mComponentRegistry->GetSchema(componentId);

            const void* componentData = e.get(componentId);

            if (componentData)
            {
                Network::ReplicationPacket::ComponentData compData;
                compData.mTypeHash = schema.mTypeHash;
                const uint8_t* dataPtr = static_cast<const uint8_t*>(componentData);
                compData.mData.assign(dataPtr, dataPtr + schema.mSize);
                packet.mComponents.push_back(std::move(compData));
            }
        }

        serverThread.QueueReplicationPacket(packet);
        rep.ClearDirty();
    });
}

}  // namespace fc
