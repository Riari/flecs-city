#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

#include <enet/enet.h>

#include "Network/Types.h"

namespace fc::Network
{

class NetworkThread
{
public:
    enum State
    {
        Idle,
        Active,
        PendingExit
    };

    NetworkThread() = default;
    ~NetworkThread();

    bool Start();
    void Stop();

protected:
    std::thread mThread;
    std::atomic<State> mState{State::Idle};

    std::atomic<ENetHost*> mHost{nullptr};

    /// @brief Connection address (when this is a client) or listen address (when this is a server).
    ENetAddress mAddress;

    std::mutex mIncomingMutex;
    std::mutex mOutgoingMutex;
    std::condition_variable mOutgoingCV;

    std::queue<NetworkMessage> mIncomingMessages;
    std::queue<NetworkMessage> mOutgoingMessages;

    virtual bool Init() = 0;
    virtual void HandleEvent(const ENetEvent& event);
    virtual void Disconnect() {}

private:
    /// @brief Thread function
    void Main();

    void ProcessOutgoingMessages();
};

} // namespace fc::Network
