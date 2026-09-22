#pragma once

#include <raylib.h>

namespace fc
{

/// @brief Wrapper for a Raylib Model.
struct ModelComponent
{
    // TODO: Store a reference to the model instead of the model data so that this component can be replicated and the model can be loaded only where it's needed (i.e. in monolith or client modes)
    Model mModel;

    float mScale = 1.f;
    Color mTint = WHITE;
};

}; // namespace fc
