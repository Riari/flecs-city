#include "Test.h"

#include <iostream>
#include <ostream>

#include <flecs.h>
#include <spdlog/spdlog.h>

#include <Core/ECS/Components/PositionComponent.h>
#include <Core/ECS/Phases.h>

namespace fc::Test
{

static void RegisterComponents(flecs::world& ecs)
{
}

static void InitECS(flecs::world& ecs)
{
    assert(fc::Draw3D.is_valid() && "Draw3D phase not initialized!");
    spdlog::info("Registering fc::Test stuff...");
    ecs.system<const fc::PositionComponent>("fc::Test::SpammySystem")
        .kind(fc::Draw3D)
        .each([](const fc::PositionComponent& position) {
            std::cout << "Hello?" << std::endl;
            spdlog::info("Spam, eggs, beans, and spam");
        });
}

Module MODULE{
    &RegisterComponents,
    &InitECS
};

};  // namespace fc::Test
