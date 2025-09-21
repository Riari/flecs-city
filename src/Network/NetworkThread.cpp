#include "NetworkThread.h"

#include <enet/enet.h>
#include <spdlog/spdlog.h>
#include <chrono>

namespace fc::Network
{

NetworkThread::~NetworkThread()
{
    Stop();
}

bool NetworkThread::Start()
{
    if (mThread.joinable())
    {
        return false;
    }

    mThread = std::thread(&NetworkThread::Main, this);

    return true;
}

void NetworkThread::Stop()
{
    mOutgoingCV.notify_all();

    if (mThread.joinable())
    {
        mThread.join();
    }
}

void NetworkThread::Main()
{
    if (enet_initialize() != 0)
    {
        spdlog::error("Failed to initialize ENet");
        return;
    }

    if (!Init())
    {
        enet_deinitialize();
        return;
    }

    State state = mState.load();
    while (state != State::PendingExit)
    {
        if (state == State::Idle)
            continue;

        ProcessOutgoingMessages();

        ENetEvent event;
        int result = enet_host_service(mHost, &event, 10);
        if (result > 0)
        {
            HandleEvent(event);
        }
        else if (result < 0)
        {
            spdlog::error("An ENet service error occurred");
            break;
        }

        // Small yield to prevent busy waiting
        // TODO: replace with a more suitable alternative (CV?)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        state = mState.load();
    }

    Disconnect();

    if (mHost)
    {
        enet_host_destroy(mHost);
    }

    enet_deinitialize();
}

void NetworkThread::ProcessOutgoingMessages()
{
    // TODO: implement me
}

void NetworkThread::HandleEvent(const ENetEvent& event)
{
    // TODO: deduplicate event handling
    switch (event.type)
    {
        case ENET_EVENT_TYPE_RECEIVE:
            {
                std::vector<uint8_t> data(event.packet->data, event.packet->data + event.packet->dataLength);

                {
                    std::lock_guard<std::mutex> lock(mIncomingMutex);
                    mIncomingMessages.push({std::move(data), event.channelID, 0});
                }

                enet_packet_destroy(event.packet);
            }

        default:
            break;
    }
}

} // namespace fc::Network
