#pragma once

#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <flecs.h>

#include "ECS/Components/ReplicatedComponent.h"
#include "Utils/Hash.h"

namespace fc::Network::Server
{

struct ComponentSchema
{
    flecs::id_t mComponentId;
    size_t mSize;
    std::string mName;
    uint32_t mTypeHash;
};

/// @brief Initialises ECS observers for marking an entity for replication when a replicated
/// component is add/set/removed
template<typename T>
void InitComponentObserver(const flecs::world& ecs)
{
    ecs.observer<T, ReplicatedComponent>()
        .event(flecs::OnAdd)
        .each([](flecs::entity e, T& component, ReplicatedComponent& rep)
        {
            // Don't mark as dirty if the entity is new - that case is handled by the
            // lifecycle OnAdd observer (see below)
            if (!rep.mNewlyCreated)
            {
                rep.MarkDirty(e.world().id<T>());
            }
        });

    ecs.observer<T, ReplicatedComponent>()
        .event(flecs::OnSet)
        .each([](flecs::entity e, T& component, ReplicatedComponent& rep)
        {
            rep.MarkDirty(e.world().id<T>());
        });

    ecs.observer<T, ReplicatedComponent>()
        .event(flecs::OnRemove)
        .each([](flecs::entity e, T& component, ReplicatedComponent& rep)
        {
            // Mark the entity as dirty so the removal can be replicated without
            // sending the component data
            rep.mDirty = true;
        });
}

/// @brief ECS singleton for tracking replicated entities due for destruction
struct ReplicationDestructionSingleton
{
    std::queue<uint64_t> mQueue;
};

void InitEntityObservers(const flecs::world& ecs)
{
    ecs.observer<ReplicatedComponent>()
        .event(flecs::OnAdd)
        .each([](flecs::entity e, ReplicatedComponent& rep)
        {
            rep.mDirty = true;
            rep.mNewlyCreated = true;
        });

    ecs.observer<ReplicatedComponent>()
        .event(flecs::OnRemove)
        .each([](flecs::entity e, ReplicatedComponent& rep)
        {
            ReplicationDestructionSingleton& singleton = e.world().get_mut<ReplicationDestructionSingleton>();

            singleton.mQueue.push(e.id());
        });
}

/// @brief ECS singleton for tracking replicated component types
struct ReplicationRegistrySingleton
{
    std::unordered_map<flecs::id_t, ComponentSchema> mIdToSchema;
    std::unordered_map<uint32_t, flecs::id_t> mHashToId;
    std::unordered_set<flecs::id_t> mComponents;

    template<typename T>
    void RegisterComponent(const std::string& name, const flecs::world& ecs)
    {
        ComponentSchema schema;
        schema.mComponentId = ecs.id<T>();
        schema.mSize = sizeof(T);
        schema.mName = name;
        schema.mTypeHash = Utils::HashString(name);

        mIdToSchema[schema.mComponentId] = schema;
        mHashToId[schema.mTypeHash] = schema.mComponentId;
        mComponents.insert(schema.mComponentId);

        InitComponentObserver<T>(ecs);
    }
};


}; // namespace fc::Network::Server
