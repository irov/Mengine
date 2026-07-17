#include "Arena3DCanonicalWriter.h"

#include "Config/StdUtility.h"

//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    void CanonicalWriter::reserve( size_t _size )
    {
        m_data.reserve( _size );
    }
    //////////////////////////////////////////////////////////////////////////
    void CanonicalWriter::clear()
    {
        m_data.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void CanonicalWriter::writeU8( uint8_t _value )
    {
        m_data.push_back( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void CanonicalWriter::writeBool( bool _value )
    {
        this->writeU8( _value ? 1u : 0u );
    }
    //////////////////////////////////////////////////////////////////////////
    void CanonicalWriter::writeU16( uint16_t _value )
    {
        for( uint32_t shift = 0; shift != 16; shift += 8 )
        {
            m_data.push_back( static_cast<uint8_t>(_value >> shift) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void CanonicalWriter::writeI16( int16_t _value )
    {
        this->writeU16( static_cast<uint16_t>(_value) );
    }
    //////////////////////////////////////////////////////////////////////////
    void CanonicalWriter::writeU32( uint32_t _value )
    {
        for( uint32_t shift = 0; shift != 32; shift += 8 )
        {
            m_data.push_back( static_cast<uint8_t>(_value >> shift) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void CanonicalWriter::writeU64( uint64_t _value )
    {
        for( uint32_t shift = 0; shift != 64; shift += 8 )
        {
            m_data.push_back( static_cast<uint8_t>(_value >> shift) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void CanonicalWriter::writeI32( int32_t _value )
    {
        this->writeU32( static_cast<uint32_t>(_value) );
    }
    //////////////////////////////////////////////////////////////////////////
    void CanonicalWriter::writeI64( int64_t _value )
    {
        this->writeU64( static_cast<uint64_t>(_value) );
    }
    //////////////////////////////////////////////////////////////////////////
    void CanonicalWriter::writeFixed( kf_fixed_t _value )
    {
        this->writeI32( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void CanonicalWriter::writeVector( const kf_vec3_t & _value )
    {
        this->writeFixed( _value.x );
        this->writeFixed( _value.y );
        this->writeFixed( _value.z );
    }
    //////////////////////////////////////////////////////////////////////////
    void CanonicalWriter::writeBytes( const void * _bytes, size_t _size )
    {
        const uint8_t * begin = static_cast<const uint8_t *>(_bytes);
        m_data.insert( m_data.end(), begin, begin + _size );
    }
    //////////////////////////////////////////////////////////////////////////
    void CanonicalWriter::writeString( const Mengine::String & _value )
    {
        this->writeU32( static_cast<uint32_t>(_value.size()) );
        this->writeBytes( _value.data(), _value.size() );
    }
    //////////////////////////////////////////////////////////////////////////
    const Mengine::Data & CanonicalWriter::data() const
    {
        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    Mengine::Data CanonicalWriter::take()
    {
        return Mengine::StdUtility::move( m_data );
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
