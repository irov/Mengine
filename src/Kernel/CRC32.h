#pragma once

#include "Config/Typedef.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        uint32_t makeCRC32( const void * _buffer, size_t _size );
        uint32_t makeCRC32Mod( const void * _buffer, size_t _size, uint32_t _mod );
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        uint32_t makeCRC32Pod( const T & _t )
        {
            uint32_t value = makeCRC32( &_t, sizeof( T ) );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        uint32_t makeCRC32ModPod( const T & _t, uint32_t _mod )
        {
            uint32_t value = makeCRC32Mod( &_t, sizeof( T ), _mod );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}