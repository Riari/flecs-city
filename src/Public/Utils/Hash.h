#pragma once

#include <string>

namespace fc::Utils
{

/// @brief FNV-1a hash function
inline uint32_t HashString(const std::string& str)
{
    uint32_t hash = 2166136261u;
    for (char c : str)
    {
        hash ^= static_cast<uint32_t>(c);
        hash *= 16777619u;
    }

    return hash;
}

}; // namespace fc::Utils
