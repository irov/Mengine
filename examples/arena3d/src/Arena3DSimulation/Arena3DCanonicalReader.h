#pragma once

#include "kinefix/kinefix.hpp"

#include "Kernel/Data.h"
#include "Kernel/String.h"

#include "Config/StdDef.h"
#include "Config/StdInt.h"

namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    class CanonicalReader
    {
    public:
        CanonicalReader( const void * _bytes, size_t _size );
        explicit CanonicalReader( const Mengine::Data & _bytes );

        bool readU8( uint8_t * _value );
        bool readBool( bool * _value );
        bool readU16( uint16_t * _value );
        bool readI16( int16_t * _value );
        bool readU32( uint32_t * _value );
        bool readU64( uint64_t * _value );
        bool readI32( int32_t * _value );
        bool readI64( int64_t * _value );
        bool readFixed( kf_fixed_t * _value );
        bool readVector( kf_vec3_t * _value );
        bool readBytes( void * _bytes, size_t _size );
        bool readString( Mengine::String * _value );

        bool valid() const;
        size_t remaining() const;

    private:
        const uint8_t * m_bytes;
        size_t m_size;
        size_t m_offset;
        bool m_valid;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
