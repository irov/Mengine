#pragma once

#include "kinefix/kinefix.hpp"

#include "Kernel/Data.h"
#include "Kernel/String.h"

#include "Config/StdDef.h"
#include "Config/StdInt.h"

namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    class CanonicalWriter
    {
    public:
        void reserve( size_t _size );
        void clear();
        void writeU8( uint8_t _value );
        void writeBool( bool _value );
        void writeU16( uint16_t _value );
        void writeI16( int16_t _value );
        void writeU32( uint32_t _value );
        void writeU64( uint64_t _value );
        void writeI32( int32_t _value );
        void writeI64( int64_t _value );
        void writeFixed( kf_fixed_t _value );
        void writeVector( const kf_vec3_t & _value );
        void writeBytes( const void * _bytes, size_t _size );
        void writeString( const Mengine::String & _value );

        const Mengine::Data & data() const;
        Mengine::Data take();

    private:
        Mengine::Data m_data;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
