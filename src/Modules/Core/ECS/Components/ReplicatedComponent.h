#pragma once

#include <unordered_set>

#include <flecs.h>

/// @brief Component for designating an entity as replicated.
struct ReplicatedComponent
{
    bool mDirty = true;
    std::unordered_set<flecs::id_t> mDirtyComponents;
    float mLastReplicatedTime{0};
    bool mNewlyCreated = true;

    void MarkDirty(flecs::id_t componentId)
    {
        mDirty = true;
        mDirtyComponents.insert(componentId);
    }

    void ClearDirty()
    {
        mDirty = false;
        mDirtyComponents.clear();
        mNewlyCreated = false;
    }

    bool IsComponentDirty(flecs::id_t componentId) const
    {
        return mNewlyCreated || mDirtyComponents.count(componentId) > 0;
    }
};
