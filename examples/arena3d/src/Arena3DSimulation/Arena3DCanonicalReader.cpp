#include "Arena3DCanonicalReader.h"

#include "Config/StdLimits.h"
#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    CanonicalReader::CanonicalReader( const void * _bytes, size_t _size )
        : m_bytes( static_cast<const uint8_t *>(_bytes) )
        , m_size( _size )
        , m_offset( 0 )
        , m_valid( _bytes != nullptr || _size == 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    CanonicalReader::CanonicalReader( const Mengine::Data & _bytes )
        : CanonicalReader( _bytes.data(), _bytes.size() )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool CanonicalReader::readBytes( void * _bytes, size_t _size )
    {
        if( m_valid == false || _size > m_size - m_offset )
        {
            m_valid = false;
            return false;
        }
        Mengine::StdString::memcpy( _bytes, m_bytes + m_offset, _size );
        m_offset += _size;
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CanonicalReader::readU8( uint8_t * _value )
    {
        return this->readBytes( _value, 1 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool CanonicalReader::readBool( bool * _value )
    {
        uint8_t encoded = 0;
        if( this->readU8( &encoded ) == false || encoded > 1 )
        {
            m_valid = false;
            return false;
        }
        *_value = encoded != 0;
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CanonicalReader::readU16( uint16_t * _value )
    {
        uint8_t data[2];
        if( this->readBytes( data, 2 ) == false )
        {
            return false;
        }
        *_value = static_cast<uint16_t>(data[0]) | static_cast<uint16_t>(data[1] << 8);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CanonicalReader::readI16( int16_t * _value )
    {
        uint16_t encoded = 0;
        if( this->readU16( &encoded ) == false )
        {
            return false;
        }
        *_value = encoded <= static_cast<uint16_t>(INT16_MAX)
            ? static_cast<int16_t>(encoded)
            : static_cast<int16_t>(static_cast<int32_t>(encoded) - INT32_C(65536));
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CanonicalReader::readU32( uint32_t * _value )
    {
        uint8_t data[4];
        if( this->readBytes( data, 4 ) == false )
        {
            return false;
        }
        *_value = static_cast<uint32_t>(data[0]) | (static_cast<uint32_t>(data[1]) << 8) | (static_cast<uint32_t>(data[2]) << 16) | (static_cast<uint32_t>(data[3]) << 24);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CanonicalReader::readU64( uint64_t * _value )
    {
        uint8_t data[8];
        if( this->readBytes( data, 8 ) == false )
        {
            return false;
        }
        uint64_t result = 0;
        for( uint32_t index = 0; index != 8; ++index )
        {
            result |= static_cast<uint64_t>(data[index]) << (index * 8);
        }
        *_value = result;
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CanonicalReader::readI32( int32_t * _value )
    {
        uint32_t encoded = 0;
        if( this->readU32( &encoded ) == false )
        {
            return false;
        }
        *_value = static_cast<int32_t>(encoded);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CanonicalReader::readI64( int64_t * _value )
    {
        uint64_t encoded = 0;
        if( this->readU64( &encoded ) == false )
        {
            return false;
        }
        *_value = static_cast<int64_t>(encoded);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CanonicalReader::readFixed( kf_fixed_t * _value )
    {
        return this->readI32( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool CanonicalReader::readVector( kf_vec3_t * _value )
    {
        return this->readFixed( &_value->x ) && this->readFixed( &_value->y ) && this->readFixed( &_value->z );
    }
    //////////////////////////////////////////////////////////////////////////
    bool CanonicalReader::readString( Mengine::String * _value )
    {
        uint32_t size = 0;
        if( this->readU32( &size ) == false || size > this->remaining() )
        {
            m_valid = false;
            return false;
        }
        _value->assign( reinterpret_cast<const char *>(m_bytes + m_offset), size );
        m_offset += size;
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CanonicalReader::valid() const
    {
        return m_valid;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t CanonicalReader::remaining() const
    {
        return m_valid ? m_size - m_offset : 0;
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
