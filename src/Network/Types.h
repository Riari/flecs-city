#pragma once

#include <cstdint>
#include <vector>

#include <enet/enet.h>

namespace fc::Network
{

struct InMessage
{
    std::vector<uint8_t> mData;
    uint32_t mChannel{0};
};

struct OutMessage
{
    std::vector<uint8_t> mData;
    ENetPeer* mPeer;
    uint32_t mChannel{0};
    uint32_t mFlags{ENET_PACKET_FLAG_RELIABLE};
};

} // namespace fc::Network
