#pragma once

#include "Arena3DCanonicalReader.h"
#include "Arena3DCanonicalWriter.h"
#include "Arena3DPcg32.h"

#include "kinefix/kinefix.hpp"

#include "Kernel/Data.h"
#include "Kernel/Vector.h"

#include "Config/StdDef.h"
#include "Config/StdInt.h"

namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    constexpr uint32_t CommandFormatVersion = 3;
    constexpr uint32_t SnapshotFormatVersion = 7;
    constexpr uint32_t ReplayFormatVersion = 6;
    constexpr uint32_t SimulationVersion = 13;
    constexpr uint32_t DeterministicMathVersion = 5;
    constexpr uint32_t ConfigFormatVersion = 3;
    constexpr uint32_t CollisionFormatVersion = 4;
    constexpr uint32_t TickMilliseconds = 10;
    //////////////////////////////////////////////////////////////////////////
    enum class CommandType : uint8_t
    {
        ButtonDown,
        ButtonUp,
        LookDelta,
        FireDown,
        FireUp,
        SelectWeapon,
        Pause,
        Resume,
        Restart,
        Noclip,
        JoinPlayer,
        LeavePlayer
    };
    //////////////////////////////////////////////////////////////////////////
    enum class InputButton : uint8_t
    {
        Forward,
        Backward,
        Left,
        Right,
        Jump,
        Crouch
    };
    //////////////////////////////////////////////////////////////////////////
    constexpr size_t WeaponTypeCount = 6;
    //////////////////////////////////////////////////////////////////////////
    enum class WeaponType : uint8_t
    {
        Nailgun,
        RocketLauncher,
        Railgun,
        Plasmagun,
        GrenadeLauncher,
        Shotgun,
        Count = WeaponTypeCount
    };
    //////////////////////////////////////////////////////////////////////////
    struct CommandPayload
    {
        InputButton button = InputButton::Forward;
        WeaponType weapon = WeaponType::Nailgun;
        kf_sangle16_t lookYaw = 0;
        kf_sangle16_t lookPitch = 0;
        bool enabled = false;
    };
    //////////////////////////////////////////////////////////////////////////
    struct CommandEnvelope
    {
        uint64_t matchId = 0;
        uint32_t playerId = 0;
        uint64_t sequence = 0;
        uint64_t sourceTimeMs = 0;
        uint64_t appliedTick = 0;
        CommandType type = CommandType::ButtonDown;
        CommandPayload payload;
        bool tickCorrected = false;
        bool accepted = true;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Mengine::Vector<CommandEnvelope> VectorCommandEnvelope;
    typedef Mengine::Vector<const CommandEnvelope *> VectorCommandEnvelopePtr;
    //////////////////////////////////////////////////////////////////////////
    uint32_t crc32( const void * bytes, size_t size );
    uint32_t crc32( const Mengine::Data & bytes );
    //////////////////////////////////////////////////////////////////////////
    uint64_t splitMix64( uint64_t value );
    void serializeCommand( CanonicalWriter * writer, const CommandEnvelope & command );
    bool deserializeCommand( CanonicalReader * reader, CommandEnvelope * command );
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
