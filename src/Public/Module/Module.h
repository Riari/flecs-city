#pragma once

#include <flecs.h>

namespace fc
{

/// @brief Function pointers for initialising a module.
struct Module
{
    /// @brief For registering ECS components. Called for all modules before @c InitECS.
    /// @param ecs The flecs world.
    void (*RegisterComponents)(flecs::world& ecs);

    /// @brief For initialising ECS entities (including systems/queries).
    /// @param ecs The flecs world.
    void (*InitECS)(flecs::world& ecs);
};

}; // namespace fc
