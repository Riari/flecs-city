#include <vector>

#include <flecs.h>
#include <raylib.h>
#include <spdlog/spdlog.h>

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#ifndef STEAMNETWORKINGSOCKETS_OPENSOURCE
#include <steam/steam_api.h>
#endif

#include <Modules/Core/Core.h>

int main()
{
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

    ISteamNetworkingSockets* socketInterface = SteamNetworkingSockets();

    SteamNetworkingIPAddr serverLocalAddr;
    serverLocalAddr.Clear();
    serverLocalAddr.m_port = 6969;

    while (!WindowShouldClose())
    {
        ecs.progress();
    }

    CloseWindow();

    return 0;
}
