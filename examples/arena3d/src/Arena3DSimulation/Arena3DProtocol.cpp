#include "Arena3DProtocol.h"

//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    uint32_t crc32( const void * bytes, size_t size )
    {
        uint32_t result = UINT32_C(0xFFFFFFFF);
        const uint8_t * data = static_cast<const uint8_t *>(bytes);
        for( size_t index = 0; index != size; ++index )
        {
            result ^= data[index];
            for( uint32_t bit = 0; bit != 8; ++bit )
            {
                const uint32_t mask = 0u - (result & 1u);
                result = (result >> 1) ^ (UINT32_C(0xEDB88320) & mask);
            }
        }
        return ~result;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t crc32( const Mengine::Data & bytes )
    {
        return crc32( bytes.data(), bytes.size() );
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t splitMix64( uint64_t value )
    {
        return kf_splitmix64( value );
    }
    //////////////////////////////////////////////////////////////////////////
    void serializeCommand( CanonicalWriter * writer, const CommandEnvelope & command )
    {
        writer->writeU32( CommandFormatVersion );
        writer->writeU64( command.matchId );
        writer->writeU32( command.playerId );
        writer->writeU64( command.sequence );
        writer->writeU64( command.sourceTimeMs );
        writer->writeU64( command.appliedTick );
        writer->writeU8( static_cast<uint8_t>(command.type) );
        writer->writeU8( static_cast<uint8_t>(command.payload.button) );
        writer->writeU8( static_cast<uint8_t>(command.payload.weapon) );
        writer->writeI16( command.payload.lookYaw );
        writer->writeI16( command.payload.lookPitch );
        writer->writeBool( command.payload.enabled );
        writer->writeBool( command.tickCorrected );
        writer->writeBool( command.accepted );
    }
    //////////////////////////////////////////////////////////////////////////
    bool deserializeCommand( CanonicalReader * reader, CommandEnvelope * command )
    {
        uint32_t version = 0;
        uint8_t type = 0;
        uint8_t button = 0;
        uint8_t weapon = 0;
        if( reader->readU32( &version ) == false || version != CommandFormatVersion ||
            reader->readU64( &command->matchId ) == false || reader->readU32( &command->playerId ) == false ||
            reader->readU64( &command->sequence ) == false || reader->readU64( &command->sourceTimeMs ) == false ||
            reader->readU64( &command->appliedTick ) == false || reader->readU8( &type ) == false ||
            reader->readU8( &button ) == false || reader->readU8( &weapon ) == false ||
            reader->readI16( &command->payload.lookYaw ) == false || reader->readI16( &command->payload.lookPitch ) == false ||
            reader->readBool( &command->payload.enabled ) == false || reader->readBool( &command->tickCorrected ) == false ||
            reader->readBool( &command->accepted ) == false )
        {
            return false;
        }
        if( type > static_cast<uint8_t>(CommandType::LeavePlayer) || button > static_cast<uint8_t>(InputButton::Crouch) || weapon >= WeaponTypeCount )
        {
            return false;
        }
        command->type = static_cast<CommandType>(type);
        command->payload.button = static_cast<InputButton>(button);
        command->payload.weapon = static_cast<WeaponType>(weapon);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
