#pragma once

#include "Arena3DSimulation.h"

#include "Kernel/String.h"
#include "Kernel/Vector.h"

#include "Config/StdInt.h"

namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    constexpr uint32_t NetworkWireMagic = UINT32_C(0x574E3341);
    constexpr uint32_t NetworkProtocolVersion = 2;
    constexpr uint32_t NetworkStateFrameTicks = 10;
    constexpr uint32_t NetworkEventHistoryTicks = 100;
    constexpr size_t NetworkMaximumFrameEvents = MaximumEventsPerTick;
    //////////////////////////////////////////////////////////////////////////
    enum class NetworkMessage : uint8_t
    {
        Hello = 1,
        Welcome,
        Reject,
        CommandBatch,
        StateFrame,
        SnapshotRequest,
        Roster
    };
    //////////////////////////////////////////////////////////////////////////
    enum class NetworkClientKind : uint8_t
    {
        Human,
        Bot
    };
    //////////////////////////////////////////////////////////////////////////
    struct NetworkRosterEntry
    {
        uint32_t playerId = 0;
        NetworkClientKind kind = NetworkClientKind::Human;
        Mengine::String name;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Mengine::Vector<NetworkRosterEntry> VectorNetworkRosterEntry;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
