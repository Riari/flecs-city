#pragma once

#include <flecs.h>

/// @brief Component for designating an entity as replicated.
struct ReplicatedComponent
{
    bool mDirty = true;
    static constexpr size_t MAX_DIRTY_COMPONENTS = 64;
    flecs::id_t mDirtyComponents[MAX_DIRTY_COMPONENTS];
    size_t mDirtyComponentCount = 0;
    float mLastReplicatedTime{0};
    bool mNewlyCreated = true;

    void MarkDirty(flecs::id_t componentId)
    {
        mDirty = true;
        for (size_t i = 0; i < mDirtyComponentCount; ++i)
        {
            if (mDirtyComponents[i] == componentId)
                return;
        }

        if (mDirtyComponentCount < MAX_DIRTY_COMPONENTS)
        {
            mDirtyComponents[mDirtyComponentCount++] = componentId;
        }
    }

    void ClearDirty()
    {
        mDirty = false;
        mDirtyComponentCount = 0;
        mNewlyCreated = false;
    }

    bool IsComponentDirty(flecs::id_t componentId) const
    {
        if (mNewlyCreated) return true;
        
        for (size_t i = 0; i < mDirtyComponentCount; ++i)
        {
            if (mDirtyComponents[i] == componentId)
                return true;
        }

        return false;
    }
};
