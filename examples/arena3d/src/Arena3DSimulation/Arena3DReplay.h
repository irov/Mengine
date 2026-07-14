#pragma once

#include "Arena3DSimulation.h"

#include <cstdint>
#include <filesystem>
#include <vector>

namespace Arena3D
{
    struct ReplayCheckpoint
    {
        uint64_t tick = 0;
        StateChecksum checksum;
        std::vector<uint8_t> snapshot;
    };

    struct ReplayRecord
    {
        uint64_t matchId = 0;
        uint64_t seed = 0;
        uint64_t finalTick = 0;
        uint32_t levelCrc = 0;
        uint32_t configCrc = 0;
        uint32_t collisionCrc = 0;
        StateChecksum finalChecksum;
        std::vector<CommandEnvelope> commands;
        std::vector<ServerEvent> events;
        std::vector<ReplayCheckpoint> checkpoints;
    };

    ReplayRecord makeReplayRecord( const Simulation & simulation, uint64_t seed, const std::vector<CommandEnvelope> & journal,
        const std::vector<ServerEvent> & events = {} );
    bool writeReplay( const std::filesystem::path & path, const ReplayRecord & replay );
    bool readReplay( const std::filesystem::path & path, ReplayRecord * replay );
    bool runReplay( const ReplayRecord & replay, Simulation * simulation, StateChecksum * finalChecksum = nullptr,
        uint64_t faultInjectionTick = UINT64_MAX );
}
