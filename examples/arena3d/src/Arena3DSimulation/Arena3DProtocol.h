#pragma once

#include "Arena3DFixed.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace Arena3D
{
    constexpr uint32_t CommandFormatVersion = 2;
    constexpr uint32_t SnapshotFormatVersion = 3;
    constexpr uint32_t ReplayFormatVersion = 3;
    constexpr uint32_t SimulationVersion = 5;
    constexpr uint32_t DeterministicMathVersion = 3;
    constexpr uint32_t ConfigFormatVersion = 1;
    constexpr uint32_t CollisionFormatVersion = 3;
    constexpr uint32_t TickMilliseconds = 10;

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
        Noclip
    };

    enum class InputButton : uint8_t
    {
        Forward,
        Backward,
        Left,
        Right,
        Jump,
        Crouch
    };

    enum class WeaponType : uint8_t
    {
        Nailgun,
        RocketLauncher,
        Railgun,
        Plasmagun,
        GrenadeLauncher,
        Shotgun,
        Count
    };

    struct CommandPayload
    {
        InputButton button = InputButton::Forward;
        WeaponType weapon = WeaponType::Nailgun;
        FixedSignedAngle lookYaw = 0;
        FixedSignedAngle lookPitch = 0;
        bool enabled = false;
    };

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

    class CanonicalWriter
    {
    public:
        void reserve( size_t size );
        void clear();
        void writeU8( uint8_t value );
        void writeBool( bool value );
        void writeU16( uint16_t value );
        void writeI16( int16_t value );
        void writeU32( uint32_t value );
        void writeU64( uint64_t value );
        void writeI32( int32_t value );
        void writeI64( int64_t value );
        void writeFixed( Fixed value );
        void writeVector( const FixedVec3 & value );
        void writeBytes( const void * bytes, size_t size );
        void writeString( const std::string & value );

        const std::vector<uint8_t> & data() const;
        std::vector<uint8_t> take();

    private:
        std::vector<uint8_t> m_data;
    };

    class CanonicalReader
    {
    public:
        CanonicalReader( const void * bytes, size_t size );
        explicit CanonicalReader( const std::vector<uint8_t> & bytes );

        bool readU8( uint8_t * value );
        bool readBool( bool * value );
        bool readU16( uint16_t * value );
        bool readI16( int16_t * value );
        bool readU32( uint32_t * value );
        bool readU64( uint64_t * value );
        bool readI32( int32_t * value );
        bool readI64( int64_t * value );
        bool readFixed( Fixed * value );
        bool readVector( FixedVec3 * value );
        bool readBytes( void * bytes, size_t size );
        bool readString( std::string * value );

        bool valid() const;
        size_t remaining() const;

    private:
        const uint8_t * m_bytes;
        size_t m_size;
        size_t m_offset;
        bool m_valid;
    };

    uint32_t crc32( const void * bytes, size_t size );
    uint32_t crc32( const std::vector<uint8_t> & bytes );

    class Pcg32
    {
    public:
        Pcg32() = default;
        Pcg32( uint64_t seed, uint64_t stream );

        void seed( uint64_t seed, uint64_t stream );
        uint32_t next();
        uint32_t bounded( uint32_t bound );

        uint64_t state() const;
        uint64_t stream() const;
        void restore( uint64_t state, uint64_t stream );

    private:
        kf_pcg32_t m_random{0, 1};
    };

    uint64_t splitMix64( uint64_t value );
    void serializeCommand( CanonicalWriter * writer, const CommandEnvelope & command );
    bool deserializeCommand( CanonicalReader * reader, CommandEnvelope * command );
}
