#include <vector>

#include <flecs.h>
#include <raylib.h>
#include <spdlog/spdlog.h>

#include <Modules/Core/Core.h>
#include <Modules/Test/Test.h>

int main()
{
    constexpr int width = 1920;
    constexpr int height = 1080;

    InitWindow(width, height, "Test window");

    SetTargetFPS(60);

    flecs::world ecs;

    spdlog::info("Created flecs::world: {}", 2);

    std::vector<fc::Module> modules{
        fc::Core::MODULE,
        fc::Test::MODULE,
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
        ecs.progress();
    }

    CloseWindow();

    return 0;
}
