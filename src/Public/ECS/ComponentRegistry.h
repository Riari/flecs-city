#pragma once

#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <flecs.h>

#include "ReplicatedComponent.h"
#include "Utils/Hash.h"

namespace fc::ECS
{

struct ComponentSchema
{
    flecs::id_t mComponentId;
    size_t mSize;
    static constexpr size_t MAX_NAME_LENGTH = 128;
    char mName[MAX_NAME_LENGTH];
    uint32_t mTypeHash;

    ComponentSchema() = default;
    ComponentSchema(const std::string& name)
    {
        strncpy(mName, name.c_str(), MAX_NAME_LENGTH - 1);
        mName[MAX_NAME_LENGTH - 1] = '\0';
    }
};

class ComponentRegistry
{
public:
    ComponentRegistry(flecs::world& ecs) : mEcs(ecs) {}

    template<typename T>
    flecs::component<T> RegisterComponent()
    {
        return mEcs.component<T>();
    }

    template<typename T>
    flecs::component<T> RegisterReplicatedComponent(const std::string& name)
    {
        flecs::component<T> type = this->RegisterComponent<T>();

        ComponentSchema schema(name);
        schema.mComponentId = mEcs.id<T>();
        schema.mSize = sizeof(T);
        schema.mTypeHash = Utils::HashString(name);

        mIdToSchema[schema.mComponentId] = schema;
        mHashToId[schema.mTypeHash] = schema.mComponentId;
        mComponents.insert(schema.mComponentId);

        this->InitComponentObserver<T>();

        return type;
    }

    ComponentSchema GetSchema(const flecs::id_t componentId)
    {
        return mIdToSchema[componentId];
    }

private:
    flecs::world& mEcs;

    std::unordered_map<flecs::id_t, ComponentSchema> mIdToSchema;
    std::unordered_map<uint32_t, flecs::id_t> mHashToId;
    std::unordered_set<flecs::id_t> mComponents;

    std::queue<uint64_t> mDestructionQueue;

    void InitEntityObservers()
    {
        mEcs.observer<ReplicatedComponent>()
            .event(flecs::OnAdd)
            .each([](flecs::entity e, ReplicatedComponent& rep)
            {
                rep.mIsDirty = true;
                rep.mIsNewEntity = true;
            });

        mEcs.observer<ReplicatedComponent>()
            .event(flecs::OnRemove)
            .each([this](flecs::entity e, ReplicatedComponent& rep)
            {
                mDestructionQueue.push(e.id());
            });
    }

    template<typename T>
    void InitComponentObserver()
    {
        mEcs.observer<T, ReplicatedComponent>()
            .event(flecs::OnAdd)
            .each([](flecs::entity e, T& component, ReplicatedComponent& rep)
            {
                // Don't mark as dirty if the entity is new - that case is handled by the
                // entity OnAdd observer (see InitEntityObservers above)
                if (!rep.mIsNewEntity)
                {
                    rep.MarkDirty(e.world().id<T>());
                }
            });

        mEcs.observer<T, ReplicatedComponent>()
            .event(flecs::OnSet)
            .each([](flecs::entity e, T& component, ReplicatedComponent& rep)
            {
                rep.MarkDirty(e.world().id<T>());
            });

        mEcs.observer<T, ReplicatedComponent>()
            .event(flecs::OnRemove)
            .each([](flecs::entity e, T& component, ReplicatedComponent& rep)
            {
                // Mark the entity as dirty so the removal can be replicated without
                // sending the component data
                rep.mIsDirty = true;
            });
    }
};

} // namespace fc::ECS
