#pragma once

#include "Arena3DSimulation.h"

#include "Kernel/Data.h"
#include "Kernel/String.h"
#include "Kernel/Vector.h"

#include "Config/StdInt.h"

namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    struct ReplayCheckpoint
    {
        uint64_t tick = 0;
        StateChecksum checksum;
        Mengine::Data snapshot;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Mengine::Vector<ReplayCheckpoint> VectorReplayCheckpoint;
    //////////////////////////////////////////////////////////////////////////
    struct ReplayRecord
    {
        uint64_t matchId = 0;
        uint64_t seed = 0;
        uint64_t finalTick = 0;
        uint32_t levelCrc = 0;
        uint32_t configCrc = 0;
        uint32_t collisionCrc = 0;
        StateChecksum finalChecksum;
        VectorCommandEnvelope commands;
        VectorServerEvent events;
        VectorReplayCheckpoint checkpoints;
    };
    //////////////////////////////////////////////////////////////////////////
    ReplayRecord makeReplayRecord( const Simulation & simulation, uint64_t seed, const VectorCommandEnvelope & journal, const VectorServerEvent & events = {} );
    bool writeReplay( const Mengine::String & path, const ReplayRecord & replay );
    bool readReplay( const Mengine::String & path, ReplayRecord * replay );
    bool runReplay( const ReplayRecord & replay, Simulation * simulation, StateChecksum * finalChecksum = nullptr );
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
