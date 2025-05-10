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

    /// @brief For initialising common ECS entities (including systems/queries).
    /// @param ecs The flecs world.
    void (*InitCommonECS)(flecs::world& ecs);

    /// @brief For initialising server-side ECS entities (including systems/queries).
    /// @param ecs The flecs world.
    void (*InitServerECS)(flecs::world& ecs);

    /// @brief For initialising client-side ECS entities (including systems/queries).
    /// @param ecs The flecs world.
    void (*InitClientECS)(flecs::world& ecs);
};

}; // namespace fc
