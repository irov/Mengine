#include "Arena3DProtocol.h"

#include <cstring>
#include <limits>

namespace Arena3D
{
    void CanonicalWriter::reserve( size_t size ) { m_data.reserve( size ); }
    void CanonicalWriter::clear() { m_data.clear(); }
    void CanonicalWriter::writeU8( uint8_t value ) { m_data.push_back( value ); }
    void CanonicalWriter::writeBool( bool value ) { this->writeU8( value ? 1u : 0u ); }
    void CanonicalWriter::writeU16( uint16_t value )
    {
        for( uint32_t shift = 0; shift != 16; shift += 8 ) { m_data.push_back( static_cast<uint8_t>(value >> shift) ); }
    }
    void CanonicalWriter::writeI16( int16_t value ) { this->writeU16( static_cast<uint16_t>(value) ); }
    void CanonicalWriter::writeU32( uint32_t value )
    {
        for( uint32_t shift = 0; shift != 32; shift += 8 ) { m_data.push_back( static_cast<uint8_t>(value >> shift) ); }
    }
    void CanonicalWriter::writeU64( uint64_t value )
    {
        for( uint32_t shift = 0; shift != 64; shift += 8 ) { m_data.push_back( static_cast<uint8_t>(value >> shift) ); }
    }
    void CanonicalWriter::writeI32( int32_t value ) { this->writeU32( static_cast<uint32_t>(value) ); }
    void CanonicalWriter::writeI64( int64_t value ) { this->writeU64( static_cast<uint64_t>(value) ); }
    void CanonicalWriter::writeFixed( Fixed value ) { this->writeI64( value.raw() ); }
    void CanonicalWriter::writeVector( const FixedVec3 & value ) { this->writeFixed( value.x ); this->writeFixed( value.y ); this->writeFixed( value.z ); }
    void CanonicalWriter::writeBytes( const void * bytes, size_t size )
    {
        const uint8_t * begin = static_cast<const uint8_t *>(bytes);
        m_data.insert( m_data.end(), begin, begin + size );
    }
    void CanonicalWriter::writeString( const std::string & value )
    {
        this->writeU32( static_cast<uint32_t>(value.size()) );
        this->writeBytes( value.data(), value.size() );
    }
    const std::vector<uint8_t> & CanonicalWriter::data() const { return m_data; }
    std::vector<uint8_t> CanonicalWriter::take() { return std::move( m_data ); }

    CanonicalReader::CanonicalReader( const void * bytes, size_t size )
        : m_bytes( static_cast<const uint8_t *>(bytes) )
        , m_size( size )
        , m_offset( 0 )
        , m_valid( bytes != nullptr || size == 0 )
    {
    }
    CanonicalReader::CanonicalReader( const std::vector<uint8_t> & bytes )
        : CanonicalReader( bytes.data(), bytes.size() )
    {
    }

    bool CanonicalReader::readBytes( void * bytes, size_t size )
    {
        if( m_valid == false || size > m_size - m_offset )
        {
            m_valid = false;
            return false;
        }
        std::memcpy( bytes, m_bytes + m_offset, size );
        m_offset += size;
        return true;
    }
    bool CanonicalReader::readU8( uint8_t * value ) { return this->readBytes( value, 1 ); }
    bool CanonicalReader::readBool( bool * value )
    {
        uint8_t encoded = 0;
        if( this->readU8( &encoded ) == false || encoded > 1 ) { m_valid = false; return false; }
        *value = encoded != 0;
        return true;
    }
    bool CanonicalReader::readU16( uint16_t * value )
    {
        uint8_t data[2]; if( this->readBytes( data, 2 ) == false ) return false;
        *value = static_cast<uint16_t>(data[0]) | static_cast<uint16_t>(data[1] << 8); return true;
    }
    bool CanonicalReader::readI16( int16_t * value )
    {
        uint16_t encoded = 0;
        if( this->readU16( &encoded ) == false ) return false;
        *value = encoded <= static_cast<uint16_t>(INT16_MAX)
            ? static_cast<int16_t>(encoded)
            : static_cast<int16_t>(static_cast<int32_t>(encoded) - INT32_C(65536));
        return true;
    }
    bool CanonicalReader::readU32( uint32_t * value )
    {
        uint8_t data[4]; if( this->readBytes( data, 4 ) == false ) return false;
        *value = static_cast<uint32_t>(data[0]) | (static_cast<uint32_t>(data[1]) << 8) |
            (static_cast<uint32_t>(data[2]) << 16) | (static_cast<uint32_t>(data[3]) << 24); return true;
    }
    bool CanonicalReader::readU64( uint64_t * value )
    {
        uint8_t data[8]; if( this->readBytes( data, 8 ) == false ) return false;
        uint64_t result = 0; for( uint32_t index = 0; index != 8; ++index ) result |= static_cast<uint64_t>(data[index]) << (index * 8);
        *value = result; return true;
    }
    bool CanonicalReader::readI32( int32_t * value ) { uint32_t encoded = 0; if( this->readU32( &encoded ) == false ) return false; *value = static_cast<int32_t>(encoded); return true; }
    bool CanonicalReader::readI64( int64_t * value ) { uint64_t encoded = 0; if( this->readU64( &encoded ) == false ) return false; *value = static_cast<int64_t>(encoded); return true; }
    bool CanonicalReader::readFixed( Fixed * value ) { int64_t raw = 0; if( this->readI64( &raw ) == false ) return false; *value = Fixed::fromRaw( raw ); return true; }
    bool CanonicalReader::readVector( FixedVec3 * value ) { return this->readFixed( &value->x ) && this->readFixed( &value->y ) && this->readFixed( &value->z ); }
    bool CanonicalReader::readString( std::string * value )
    {
        uint32_t size = 0; if( this->readU32( &size ) == false || size > this->remaining() ) { m_valid = false; return false; }
        value->assign( reinterpret_cast<const char *>(m_bytes + m_offset), size ); m_offset += size; return true;
    }
    bool CanonicalReader::valid() const { return m_valid; }
    size_t CanonicalReader::remaining() const { return m_valid ? m_size - m_offset : 0; }

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
    uint32_t crc32( const std::vector<uint8_t> & bytes ) { return crc32( bytes.data(), bytes.size() ); }

    Pcg32::Pcg32( uint64_t seedValue, uint64_t streamValue ) { this->seed( seedValue, streamValue ); }
    void Pcg32::seed( uint64_t seedValue, uint64_t streamValue )
    {
        kf_pcg32_seed( &m_random, seedValue, streamValue );
    }
    uint32_t Pcg32::next() { return kf_pcg32_next( &m_random ); }
    uint32_t Pcg32::bounded( uint32_t bound ) { return kf_pcg32_bounded( &m_random, bound ); }
    uint64_t Pcg32::state() const { return m_random.state; }
    uint64_t Pcg32::stream() const { return m_random.increment; }
    void Pcg32::restore( uint64_t stateValue, uint64_t streamValue ) { kf_pcg32_restore( &m_random, stateValue, streamValue ); }

    uint64_t splitMix64( uint64_t value )
    {
        return kf_splitmix64( value );
    }

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
        if( type > static_cast<uint8_t>(CommandType::Noclip) || button > static_cast<uint8_t>(InputButton::Crouch) || weapon >= static_cast<uint8_t>(WeaponType::Count) )
        {
            return false;
        }
        command->type = static_cast<CommandType>(type);
        command->payload.button = static_cast<InputButton>(button);
        command->payload.weapon = static_cast<WeaponType>(weapon);
        return true;
    }
}
