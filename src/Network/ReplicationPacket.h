#pragma once

#include <cstdint>
#include <vector>

#include <enet/enet.h>
#include <flecs.h>

#include "ECS/ReplicatedComponent.h"
#include "ECS/ComponentRegistry.h"

namespace fc::Network
{

struct ReplicationPacket
{
    // Serialized fields
    uint64_t mEntityId;
    bool mIsNewEntity;
    bool mIsDestroyed;

    ENetPeer* mRecipient = nullptr;

    struct ComponentData
    {
        uint32_t mTypeHash;
        std::vector<uint8_t> mData;
    };

    std::vector<ComponentData> mComponents;

    std::vector<uint8_t> Serialize()
    {
        std::vector<uint8_t> buffer;
        buffer.reserve(256);

        auto Write = [&buffer](const void* data, size_t size) {
            const uint8_t* bytes = static_cast<const uint8_t*>(data);
            buffer.insert(buffer.end(), bytes, bytes + size);
        };

        Write(&mEntityId, sizeof(mEntityId));

        uint8_t isNew = mIsNewEntity ? 1 : 0;
        uint8_t isDestroyed = mIsDestroyed ? 1 : 0;
        Write(&isNew, 1);
        Write(&isDestroyed, 1);

        uint16_t componentCount = static_cast<uint16_t>(mComponents.size());
        Write(&componentCount, sizeof(componentCount));

        for (const auto& comp : mComponents)
        {
            Write(&comp.mTypeHash, sizeof(comp.mTypeHash));

            uint16_t dataSize = static_cast<uint16_t>(comp.mData.size());
            Write(&dataSize, sizeof(dataSize));

            buffer.insert(buffer.end(), comp.mData.begin(), comp.mData.end());
        }

        return buffer;
    }

    static ReplicationPacket Deserialize(const std::vector<uint8_t>& buffer)
    {
        ReplicationPacket packet;
        size_t offset = 0;

        auto Read = [&buffer, &offset](void* dest, size_t size) {
            memcpy(dest, buffer.data() + offset, size);
            offset += size;
        };

        Read(&packet.mEntityId, sizeof(packet.mEntityId));

        uint8_t isNew, isDestroyed;
        Read(&isNew, 1);
        Read(&isDestroyed, 1);
        packet.mIsNewEntity = (isNew != 0);
        packet.mIsDestroyed = (isDestroyed != 0);

        uint16_t componentCount;
        Read(&componentCount, sizeof(componentCount));

        for (uint16_t i = 0; i < componentCount; ++i)
        {
            ReplicationPacket::ComponentData comp;

            Read(&comp.mTypeHash, sizeof(comp.mTypeHash));

            uint16_t dataSize;
            Read(&dataSize, sizeof(dataSize));

            comp.mData.resize(dataSize);
            memcpy(comp.mData.data(), buffer.data() + offset, dataSize);
            offset += dataSize;

            packet.mComponents.push_back(std::move(comp));
        }

        return packet;
    }
};

inline ReplicationPacket GenerateReplicationPacket(flecs::entity e, const ReplicatedComponent& rep, const std::vector<flecs::id_t>& componentIds, const fc::ECS::ComponentRegistry* registry)
{
    Network::ReplicationPacket packet;
    packet.mEntityId = e.id();
    packet.mIsNewEntity = rep.mIsNewEntity;

    for (flecs::id_t componentId : componentIds)
    {
        auto schema = registry->GetSchema(componentId);
        const void* componentData = e.get(componentId);
        if (componentData)
        {
            Network::ReplicationPacket::ComponentData compData;
            compData.mTypeHash = schema.mTypeHash;
            const uint8_t* dataPtr = static_cast<const uint8_t*>(componentData);
            compData.mData.assign(dataPtr, dataPtr + schema.mSize);
            packet.mComponents.push_back(std::move(compData));
        }
    }

    return packet;
}

}  // namespace fc::Network
